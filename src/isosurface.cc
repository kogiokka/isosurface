#include "isosurface.h"

Isosurface::Isosurface()
  : h_(1.f)
  , target_value_(80)
  , vertex_count_(0)
{}

Isosurface::~Isosurface() {}

void
Isosurface::CalculateGradient()
{
  int const xsize = dimensions_[0];
  int const ysize = dimensions_[1];
  int const zsize = dimensions_[2];

  for (int z = 0; z < zsize; ++z) {
    for (int y = 0; y < ysize; ++y) {
      for (int x = 0; x < xsize; ++x) {
        float x_diff;
        float y_diff;
        float z_diff;
        if (x == 0) {
          x_diff = ForwardDifference(Value(0, y, z), Value(1, y, z));
        } else if (x == xsize - 1) {
          x_diff = BackwardDifference(Value(x - 1, y, z), Value(x, y, z));
        } else {
          x_diff = CenteredDifference(Value(x - 1, y, z), Value(x + 1, y, z));
        }
        if (y == 0) {
          y_diff = ForwardDifference(Value(x, 0, z), Value(x, 1, z));
        } else if (y == ysize - 1) {
          y_diff = BackwardDifference(Value(x, y - 1, z), Value(x, y, z));
        } else {
          y_diff = CenteredDifference(Value(x, y - 1, z), Value(x, y + 1, z));
        }
        if (z == 0) {
          z_diff = ForwardDifference(Value(x, y, 0), Value(x, y, 1));
        } else if (z == zsize - 1) {
          z_diff = BackwardDifference(Value(x, y, z - 1), Value(x, y, z));
        } else {
          z_diff = CenteredDifference(Value(x, y, z - 1), Value(x, y, z + 1));
        }

        gradients_.emplace_back(x_diff, y_diff, z_diff);
      }
    }
  }
}

void
Isosurface::MarchingCube()
{
  int const xsize = dimensions_[0];
  int const ysize = dimensions_[1];
  int const zsize = dimensions_[2];

  render_data_.reserve(xsize * ysize * zsize * 2);

  for (int z = 0; z < zsize - 1; ++z) {
    for (int y = 0; y < ysize - 1; ++y) {
      for (int x = 0; x < xsize - 1; ++x) {
        GridCell cell(x, y, z);
        unsigned short cube_index = 0;

        if (Value(cell[0]) < target_value_)
          cube_index |= 1;
        if (Value(cell[1]) < target_value_)
          cube_index |= 2;
        if (Value(cell[2]) < target_value_)
          cube_index |= 4;
        if (Value(cell[3]) < target_value_)
          cube_index |= 8;
        if (Value(cell[4]) < target_value_)
          cube_index |= 16;
        if (Value(cell[5]) < target_value_)
          cube_index |= 32;
        if (Value(cell[6]) < target_value_)
          cube_index |= 64;
        if (Value(cell[7]) < target_value_)
          cube_index |= 128;

        if (table::kEdgeTable.at(cube_index) == 0)
          continue;

        std::array<glm::vec3, 12> vert_list;
        std::array<glm::vec3, 12> grad_list;
        if (table::kEdgeTable[cube_index] & 1) {
          vert_list[0] = InterpolatedVertex(cell[0], cell[1]);
          grad_list[0] = InterpolatedNormal(cell[0], cell[1]);
        }
        if (table::kEdgeTable[cube_index] & 2) {
          vert_list[1] = InterpolatedVertex(cell[1], cell[2]);
          grad_list[1] = InterpolatedNormal(cell[1], cell[2]);
        }
        if (table::kEdgeTable[cube_index] & 4) {
          vert_list[2] = InterpolatedVertex(cell[2], cell[3]);
          grad_list[2] = InterpolatedNormal(cell[2], cell[3]);
        }
        if (table::kEdgeTable[cube_index] & 8) {
          vert_list[3] = InterpolatedVertex(cell[3], cell[0]);
          grad_list[3] = InterpolatedNormal(cell[3], cell[0]);
        }
        if (table::kEdgeTable[cube_index] & 16) {
          vert_list[4] = InterpolatedVertex(cell[4], cell[5]);
          grad_list[4] = InterpolatedNormal(cell[4], cell[5]);
        }
        if (table::kEdgeTable[cube_index] & 32) {
          vert_list[5] = InterpolatedVertex(cell[5], cell[6]);
          grad_list[5] = InterpolatedNormal(cell[5], cell[6]);
        }
        if (table::kEdgeTable[cube_index] & 64) {
          vert_list[6] = InterpolatedVertex(cell[6], cell[7]);
          grad_list[6] = InterpolatedNormal(cell[6], cell[7]);
        }
        if (table::kEdgeTable[cube_index] & 128) {
          vert_list[7] = InterpolatedVertex(cell[7], cell[4]);
          grad_list[7] = InterpolatedNormal(cell[7], cell[4]);
        }
        if (table::kEdgeTable[cube_index] & 256) {
          vert_list[8] = InterpolatedVertex(cell[0], cell[4]);
          grad_list[8] = InterpolatedNormal(cell[0], cell[4]);
        }
        if (table::kEdgeTable[cube_index] & 512) {
          vert_list[9] = InterpolatedVertex(cell[1], cell[5]);
          grad_list[9] = InterpolatedNormal(cell[1], cell[5]);
        }
        if (table::kEdgeTable[cube_index] & 1024) {
          vert_list[10] = InterpolatedVertex(cell[2], cell[6]);
          grad_list[10] = InterpolatedNormal(cell[2], cell[6]);
        }
        if (table::kEdgeTable[cube_index] & 2048) {
          vert_list[11] = InterpolatedVertex(cell[3], cell[7]);
          grad_list[11] = InterpolatedNormal(cell[3], cell[7]);
        }

        for (int i = 0; i < 16; ++i) {
          if (table::kTriTable.at(cube_index)[i] < 0)
            break;
          render_data_.push_back(vert_list[table::kTriTable[cube_index][i]].x);
          render_data_.push_back(vert_list[table::kTriTable[cube_index][i]].y);
          render_data_.push_back(vert_list[table::kTriTable[cube_index][i]].z);
          render_data_.push_back(grad_list[table::kTriTable[cube_index][i]].x);
          render_data_.push_back(grad_list[table::kTriTable[cube_index][i]].y);
          render_data_.push_back(grad_list[table::kTriTable[cube_index][i]].z);
          vertex_count_ += 3;
        }
      }
    }
  }
}

glm::vec3
Isosurface::InterpolatedVertex(glm::vec3 const& v1, glm::vec3 const& v2)
{
  float const value1 = Value(v1.x, v1.y, v1.z);
  float const value2 = Value(v2.x, v2.y, v2.z);
  float const ratio = (target_value_ - value1) / (value2 - value1);

  glm::vec3 v;
  v.x = (v2.x - v1.x) * h_ * ratio + v1.x;
  v.y = (v2.y - v1.y) * h_ * ratio + v1.y;
  v.z = (v2.z - v1.z) * h_ * ratio + v1.z;

  return v;
}

glm::vec3
Isosurface::InterpolatedNormal(glm::vec3 const& v1, glm::vec3 const& v2)
{
  float const value1 = Value(v1.x, v1.y, v1.z);
  float const value2 = Value(v2.x, v2.y, v2.z);
  float const ratio = (target_value_ - value1) / (value2 - value1);

  glm::vec3 const& g1 = Gradient(v1.x, v1.y, v1.z);
  glm::vec3 const& g2 = Gradient(v2.x, v2.y, v2.z);
  return ratio * (g2 - g1) - g1;
}

void
Isosurface::ReadRaw(std::string_view const filepath)
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

  isovalues_.assign(std::make_move_iterator(begin(tmp)),
                    std::make_move_iterator(end(tmp)));
}

inline unsigned short
Isosurface::Value(int x, int y, int z) const
{
  return static_cast<unsigned short>(isovalues_.at(Index(x, y, z)));
}

inline unsigned short
Isosurface::Value(glm::vec3 const& v) const
{
  return static_cast<unsigned short>(isovalues_.at(Index(v.x, v.y, v.z)));
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
  return static_cast<float>(back - front) / 2 * h_;
}

inline float
Isosurface::BackwardDifference(unsigned short self, unsigned short back) const
{
  return static_cast<float>(back - self) / h_;
}

inline float
Isosurface::ForwardDifference(unsigned short self, unsigned short front) const
{
  return BackwardDifference(front, self);
}

void
Isosurface::ReadRawInfo(std::string_view const filepath)
{
  std::ifstream file(filepath.data(), std::ios::binary);
  if (file.fail()) {
    fprintf(stderr, "Failed to read raw file.\n");
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

float const*
Isosurface::RenderData() const
{
  return render_data_.data();
}

unsigned int
Isosurface::RenderVertexCount() const
{
  return vertex_count_;
}

Isosurface::GridCell::GridCell(int x, int y, int z)
  : x_(x)
  , y_(y)
  , z_(z)
{
  voxel_index_[0] = glm::vec3{ x_, y_, z_ };
  voxel_index_[1] = glm::vec3{ x_ + 1, y_, z_ };
  voxel_index_[2] = glm::vec3{ x_ + 1, y_, z_ + 1 };
  voxel_index_[3] = glm::vec3{ x_, y_, z_ + 1 };
  voxel_index_[4] = glm::vec3{ x_, y_ + 1, z_ };
  voxel_index_[5] = glm::vec3{ x_ + 1, y_ + 1, z_ };
  voxel_index_[6] = glm::vec3{ x_ + 1, y_ + 1, z_ + 1 };
  voxel_index_[7] = glm::vec3{ x_, y_ + 1, z_ + 1 };
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
