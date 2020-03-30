#include "isosurface.h"

Isosurface::Isosurface()
  : h_(1.f)
  , target_value_(90)
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
  unsigned short cube_index = 0;

  vertices_.reserve(xsize * ysize * zsize);
  normals_.reserve(xsize * ysize * zsize);

  for (int z = 0; z < zsize; z += 2) {
    for (int y = 0; y < ysize; y += 2) {
      for (int x = 0; x < xsize; x += 2) {
        if (Value(x, y, z) < target_value_)
          cube_index |= 0b0000'0001;
        if (Value(x + 1, y, z) < target_value_)
          cube_index |= 0b0000'0010;
        if (Value(x + 1, y, z + 1) < target_value_)
          cube_index |= 0b0000'0100;
        if (Value(x, y, z + 1) < target_value_)
          cube_index |= 0b0000'1000;
        if (Value(x, y + 1, z) < target_value_)
          cube_index |= 0b0001'0000;
        if (Value(x + 1, y + 1, z) < target_value_)
          cube_index |= 0b0010'0000;
        if (Value(x + 1, y + 1, z + 1) < target_value_)
          cube_index |= 0b0100'0000;
        if (Value(x, y + 1, z + 1) < target_value_)
          cube_index |= 0b1000'0000;

        if (table::kEdgeTable[cube_index] == 0)
          continue;

        if (table::kEdgeTable[cube_index] & 1) {
          glm::vec3 const e1 = EdgeIndex(0, x, y, z);
          glm::vec3 const e2 = EdgeIndex(1, x, y, z);
          vertices_.push_back(InterpolatedVertex(e1, e2));
          gradients_.push_back(InterpolatedNormal(e1, e2));
        }
        if (table::kEdgeTable[cube_index] & 2) {
          glm::vec3 const e1 = EdgeIndex(1, x, y, z);
          glm::vec3 const e2 = EdgeIndex(2, x, y, z);
          vertices_.push_back(InterpolatedVertex(e1, e2));
          gradients_.push_back(InterpolatedNormal(e1, e2));
        }
        if (table::kEdgeTable[cube_index] & 4) {
          glm::vec3 const e1 = EdgeIndex(2, x, y, z);
          glm::vec3 const e2 = EdgeIndex(3, x, y, z);
          vertices_.push_back(InterpolatedVertex(e1, e2));
          gradients_.push_back(InterpolatedNormal(e1, e2));
        }
        if (table::kEdgeTable[cube_index] & 8) {
          glm::vec3 const e1 = EdgeIndex(3, x, y, z);
          glm::vec3 const e2 = EdgeIndex(0, x, y, z);
          vertices_.push_back(InterpolatedVertex(e1, e2));
          gradients_.push_back(InterpolatedNormal(e1, e2));
        }
        if (table::kEdgeTable[cube_index] & 16) {
          glm::vec3 const e1 = EdgeIndex(4, x, y, z);
          glm::vec3 const e2 = EdgeIndex(5, x, y, z);
          vertices_.push_back(InterpolatedVertex(e1, e2));
          gradients_.push_back(InterpolatedNormal(e1, e2));
        }
        if (table::kEdgeTable[cube_index] & 32) {
          glm::vec3 const e1 = EdgeIndex(5, x, y, z);
          glm::vec3 const e2 = EdgeIndex(6, x, y, z);
          vertices_.push_back(InterpolatedVertex(e1, e2));
          gradients_.push_back(InterpolatedNormal(e1, e2));
        }
        if (table::kEdgeTable[cube_index] & 64) {
          glm::vec3 const e1 = EdgeIndex(6, x, y, z);
          glm::vec3 const e2 = EdgeIndex(7, x, y, z);
          vertices_.push_back(InterpolatedVertex(e1, e2));
          gradients_.push_back(InterpolatedNormal(e1, e2));
        }
        if (table::kEdgeTable[cube_index] & 128) {
          glm::vec3 const e1 = EdgeIndex(7, x, y, z);
          glm::vec3 const e2 = EdgeIndex(4, x, y, z);
          vertices_.push_back(InterpolatedVertex(e1, e2));
          gradients_.push_back(InterpolatedNormal(e1, e2));
        }
        if (table::kEdgeTable[cube_index] & 256) {
          glm::vec3 const e1 = EdgeIndex(0, x, y, z);
          glm::vec3 const e2 = EdgeIndex(4, x, y, z);
          vertices_.push_back(InterpolatedVertex(e1, e2));
          gradients_.push_back(InterpolatedNormal(e1, e2));
        }
        if (table::kEdgeTable[cube_index] & 512) {
          glm::vec3 const e1 = EdgeIndex(1, x, y, z);
          glm::vec3 const e2 = EdgeIndex(5, x, y, z);
          vertices_.push_back(InterpolatedVertex(e1, e2));
          gradients_.push_back(InterpolatedNormal(e1, e2));
        }
        if (table::kEdgeTable[cube_index] & 1024) {
          glm::vec3 const e1 = EdgeIndex(2, x, y, z);
          glm::vec3 const e2 = EdgeIndex(6, x, y, z);
          vertices_.push_back(InterpolatedVertex(e1, e2));
          gradients_.push_back(InterpolatedNormal(e1, e2));
        }
        if (table::kEdgeTable[cube_index] & 2048) {
          glm::vec3 const e1 = EdgeIndex(3, x, y, z);
          glm::vec3 const e2 = EdgeIndex(7, x, y, z);
          vertices_.push_back(InterpolatedVertex(e1, e2));
          gradients_.push_back(InterpolatedNormal(e1, e2));
        }
      }
    }
  }
}

glm::vec3
Isosurface::InterpolatedVertex(glm::vec3 idx1, glm::vec3 idx2)
{
  float const value1 = Value(idx1.x, idx1.y, idx1.z);
  float const value2 = Value(idx2.x, idx2.y, idx2.z);
  float const ratio = (target_value_ - value1) / (value2 - value1);

  glm::vec3 v;
  v.x = (idx2.x - idx1.x) * h_ * ratio + idx1.x;
  v.y = (idx2.y - idx1.y) * h_ * ratio + idx1.y;
  v.z = (idx2.z - idx1.z) * h_ * ratio + idx1.z;

  return v;
}

glm::vec3
Isosurface::InterpolatedNormal(glm::vec3 idx1, glm::vec3 idx2)
{
  float const value1 = Value(idx1.x, idx1.y, idx1.z);
  float const value2 = Value(idx2.x, idx2.y, idx2.z);
  float const ratio = (target_value_ - value1) / (value2 - value1);

  glm::vec3 const& g1 = Gradient(idx1.x, idx1.y, idx1.z);
  glm::vec3 const& g2 = Gradient(idx2.x, idx2.y, idx2.z);
  return ratio * (g2 - g1) - g1;
}

inline unsigned short
Isosurface::Value(int x, int y, int z) const
{
  return static_cast<unsigned short>(isovalues_.at(Index(x, y, z)));
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

float
Isosurface::CenteredDifference(unsigned short front, unsigned short back) const
{
  return static_cast<float>(back - front) / 2 * h_;
}

float
Isosurface::BackwardDifference(unsigned short self, unsigned short back) const
{
  return static_cast<float>(back - self) / h_;
}

float
Isosurface::ForwardDifference(unsigned short self, unsigned short front) const
{
  return BackwardDifference(front, self);
}

void
Isosurface::ReadRaw(std::string_view const filepath)
{
  assert(!dimensions_.empty());

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

  std::stringstream ss(attrs.at(0));
  std::string dimen;
  while (std::getline(ss, dimen, ':')) {
    dimensions_.push_back(std::stoi(dimen));
  }
}

glm::vec3
Isosurface::EdgeIndex(int index, int x, int y, int z) const
{
  switch (index) {
    case 0:
      return glm::vec3{ x, y, z };
    case 1:
      return glm::vec3{ x + 1, y, z };
    case 2:
      return glm::vec3{ x + 1, y, z + 1 };
    case 3:
      return glm::vec3{ x, y, z + 1 };
    case 4:
      return glm::vec3{ x, y + 1, z };
    case 5:
      return glm::vec3{ x + 1, y + 1, z };
    case 6:
      return glm::vec3{ x + 1, y + 1, z + 1 };
    case 7:
      return glm::vec3{ x, y + 1, z + 1 };
    default:
      fprintf(stderr, "Wrong edge index: %d\n", index);
      return glm::vec3{ -1, -1, -1 };
  }
}
