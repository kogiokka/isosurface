#include "isosurface.h"

Isosurface::Isosurface(float isovalue)
  : isovalue_(isovalue)
{}

Isosurface::~Isosurface() {}

void
Isosurface::CalculateGradient()
{
  int const xsize = dimensions_[0];
  int const ysize = dimensions_[1];
  int const zsize = dimensions_[2];

  gradients_.reserve(xsize * ysize * zsize);

  for (int z = 0; z < zsize; ++z) {
    for (int y = 0; y < ysize; ++y) {
      for (int x = 0; x < xsize; ++x) {
        float x_diff;
        float y_diff;
        float z_diff;
        if (x == 0) {
          x_diff = ForwardDifference(Value(0, y, z), Value(1, y, z));
        } else if (x == xsize - 1) {
          x_diff = BackwardDifference(Value(x, y, z), Value(x - 1, y, z));
        } else {
          x_diff = CenteredDifference(Value(x - 1, y, z), Value(x + 1, y, z));
        }
        if (y == 0) {
          y_diff = ForwardDifference(Value(x, 0, z), Value(x, 1, z));
        } else if (y == ysize - 1) {
          y_diff = BackwardDifference(Value(x, y, z), Value(x, y - 1, z));
        } else {
          y_diff = CenteredDifference(Value(x, y - 1, z), Value(x, y + 1, z));
        }
        if (z == 0) {
          z_diff = ForwardDifference(Value(x, y, 0), Value(x, y, 1));
        } else if (z == zsize - 1) {
          z_diff = BackwardDifference(Value(x, y, z), Value(x, y, z - 1));
        } else {
          z_diff = CenteredDifference(Value(x, y, z - 1), Value(x, y, z + 1));
        }

        if (x_diff == 0 && y_diff == 0 && z_diff == 0) {
          gradients_.emplace_back(0.f, 0.f, 0.f);
        } else {
          gradients_.push_back(
            glm::normalize(glm::vec3{ x_diff, y_diff, z_diff }));
        }
      }
    }
  }
}

std::pair<unsigned int, std::vector<float>>
Isosurface::MarchingCube()
{
  unsigned int vertex_count = 0;
  std::vector<float> data;

  int const& xsize = dimensions_[0];
  int const& ysize = dimensions_[1];
  int const& zsize = dimensions_[2];

  data.reserve(xsize * ysize * zsize * 2);

  for (int z = 0; z < zsize - 1; ++z) {
    for (int y = 0; y < ysize - 1; ++y) {
      for (int x = 0; x < xsize - 1; ++x) {
        GridCell cell(x, y, z);
        unsigned short cube_index = 0;

        if (Value(cell[0]) < isovalue_)
          cube_index |= 1;
        if (Value(cell[1]) < isovalue_)
          cube_index |= 2;
        if (Value(cell[2]) < isovalue_)
          cube_index |= 4;
        if (Value(cell[3]) < isovalue_)
          cube_index |= 8;
        if (Value(cell[4]) < isovalue_)
          cube_index |= 16;
        if (Value(cell[5]) < isovalue_)
          cube_index |= 32;
        if (Value(cell[6]) < isovalue_)
          cube_index |= 64;
        if (Value(cell[7]) < isovalue_)
          cube_index |= 128;

        if (table::kEdgeTable[cube_index] == 0)
          continue;

        std::array<std::array<float, 6>, 12> vert_attribs;
        if (table::kEdgeTable[cube_index] & 1)
          vert_attribs[0] = InterpVertexAttribs(cell[0], cell[1]);
        if (table::kEdgeTable[cube_index] & 2)
          vert_attribs[1] = InterpVertexAttribs(cell[1], cell[2]);
        if (table::kEdgeTable[cube_index] & 4)
          vert_attribs[2] = InterpVertexAttribs(cell[2], cell[3]);
        if (table::kEdgeTable[cube_index] & 8)
          vert_attribs[3] = InterpVertexAttribs(cell[3], cell[0]);
        if (table::kEdgeTable[cube_index] & 16)
          vert_attribs[4] = InterpVertexAttribs(cell[4], cell[5]);
        if (table::kEdgeTable[cube_index] & 32)
          vert_attribs[5] = InterpVertexAttribs(cell[5], cell[6]);
        if (table::kEdgeTable[cube_index] & 64)
          vert_attribs[6] = InterpVertexAttribs(cell[6], cell[7]);
        if (table::kEdgeTable[cube_index] & 128)
          vert_attribs[7] = InterpVertexAttribs(cell[7], cell[4]);
        if (table::kEdgeTable[cube_index] & 256)
          vert_attribs[8] = InterpVertexAttribs(cell[0], cell[4]);
        if (table::kEdgeTable[cube_index] & 512)
          vert_attribs[9] = InterpVertexAttribs(cell[1], cell[5]);
        if (table::kEdgeTable[cube_index] & 1024)
          vert_attribs[10] = InterpVertexAttribs(cell[2], cell[6]);
        if (table::kEdgeTable[cube_index] & 2048)
          vert_attribs[11] = InterpVertexAttribs(cell[3], cell[7]);

        for (int i = 0; table::kTriTable[cube_index][i] != -1; ++i) {
          auto const& tri_list = table::kTriTable[cube_index];
          std::array<float, 6> const& attrbs = vert_attribs[tri_list[i]];
          for (auto const& a : attrbs) {
            data.push_back(a);
          }
          vertex_count += 3;
        }
      }
    }
  }
  return std::make_pair(vertex_count, data);
}

void
Isosurface::ReadInfo(const std::string_view filepath)
{
  std::ifstream file(filepath.data(), std::ios::binary);
  if (file.fail()) {
    std::fprintf(stderr, "Failed to read raw file.\n");
    exit(EXIT_FAILURE);
  }
  std::string value;
  std::vector<std::string> attrs;
  while (std::getline(file, value)) {
    std::string::size_type n = value.find("=");
    if (n == std::string::npos)
      continue;
    attrs.push_back(value.substr(n + 1));
  }

  std::istringstream ss(attrs.at(0));
  std::string dimen;
  int index = 0;
  while (std::getline(ss, dimen, ':')) {
    dimensions_[index] = std::stoi(dimen);
    ++index;
  }
}

void
Isosurface::ReadRaw(const std::string_view filepath)
{
  assert(dimensions_[0] > 0);
  assert(dimensions_[1] > 0);
  assert(dimensions_[2] > 0);

  std::ifstream file(filepath.data(), std::ios::binary);
  if (file.fail()) {
    fprintf(stderr, "Failed to read raw file.\n");
    exit(EXIT_FAILURE);
  }

  auto file_size = std::filesystem::file_size(filepath);

  std::vector<char> tmp(file_size);
  file.read(tmp.data(), file_size);
  file.close();

  scalar_field_.assign(std::make_move_iterator(begin(tmp)),
                       std::make_move_iterator(end(tmp)));
}

std::array<float, 6>
Isosurface::InterpVertexAttribs(glm::vec3 const& v1, glm::vec3 const& v2)
{
  constexpr float min = 0.0001;
  float const value1 = Value(v1.x, v1.y, v1.z);
  float const value2 = Value(v2.x, v2.y, v2.z);
  float const ratio = (isovalue_ - value1) / (value2 - value1);

  glm::vec3 const& g1 = Gradient(v1.x, v1.y, v1.z);
  glm::vec3 const& g2 = Gradient(v2.x, v2.y, v2.z);

  if (std::abs(value1 - value2) < min) {
    glm::vec3 const v = (v1 + v2) * 0.5f;
    glm::vec3 const g = glm::normalize((g1 + g2) * 0.5f);
    return std::array<float, 6>{ v.x, v.y, v.z, g.x, g.y, g.z };
  }
  if (std::abs(isovalue_ - value1) < min) {
    return std::array<float, 6>{ v1.x, v1.y, v1.z, g1.x, g1.y, g1.z };
  }
  if (std::abs(isovalue_ - value2) < min) {
    return std::array<float, 6>{ v2.x, v2.y, v2.z, g2.x, g2.y, g2.z };
  }

  glm::vec3 const g = glm::normalize(ratio * (g2 - g1) + g1);
  glm::vec3 const v = ratio * (v2 - v1) + v1;

  return std::array<float, 6>{ v.x, v.y, v.z, g.x, g.y, g.z };
}

inline unsigned short
Isosurface::Value(int x, int y, int z) const
{
  return static_cast<unsigned short>(scalar_field_.at(Index(x, y, z)));
}

inline unsigned short
Isosurface::Value(glm::vec3 const& v) const
{
  return static_cast<unsigned short>(scalar_field_.at(Index(v.x, v.y, v.z)));
}

inline glm::vec3 const&
Isosurface::Gradient(int x, int y, int z) const
{
  return gradients_.at(Index(x, y, z));
}

inline int
Isosurface::Index(int x, int y, int z) const
{
  int const xsize = dimensions_[0];
  int const ysize = dimensions_[1];
  return x + y * xsize + z * ysize * xsize;
}

inline float
Isosurface::CenteredDifference(unsigned short front, unsigned short back) const
{
  return (static_cast<float>(back) - static_cast<float>(front)) / 2;
}

inline float
Isosurface::BackwardDifference(unsigned short self, unsigned short back) const
{
  return (static_cast<float>(back) - static_cast<float>(self));
}

inline float
Isosurface::ForwardDifference(unsigned short self, unsigned short front) const
{
  return BackwardDifference(front, self);
}

Isosurface::GridCell::GridCell(int x, int y, int z)
  : x_(x)
  , y_(y)
  , z_(z)
{
  using namespace glm;

  voxel_index_[0] = vec3{ x_, y_, z_ };
  voxel_index_[1] = vec3{ x_ + 1, y_, z_ };
  voxel_index_[2] = vec3{ x_ + 1, y_, z_ + 1 };
  voxel_index_[3] = vec3{ x_, y_, z_ + 1 };
  voxel_index_[4] = vec3{ x_, y_ + 1, z_ };
  voxel_index_[5] = vec3{ x_ + 1, y_ + 1, z_ };
  voxel_index_[6] = vec3{ x_ + 1, y_ + 1, z_ + 1 };
  voxel_index_[7] = vec3{ x_, y_ + 1, z_ + 1 };
};

Isosurface::GridCell::~GridCell(){};

glm::vec3 const& Isosurface::GridCell::operator[](int index) const
{
  switch (index) {
    case 0:
      return voxel_index_[0];
    case 1:
      return voxel_index_[1];
    case 2:
      return voxel_index_[2];
    case 3:
      return voxel_index_[3];
    case 4:
      return voxel_index_[4];
    case 5:
      return voxel_index_[5];
    case 6:
      return voxel_index_[6];
    case 7:
      return voxel_index_[7];
    default:
      fprintf(stderr, "Wrong gridcell index: %d\n", index);
      exit(EXIT_FAILURE);
  }
}
