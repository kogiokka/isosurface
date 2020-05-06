#include "isosurface.h"

Isosurface::Isosurface(std::vector<float> scalar_field)
  : scalar_field_(scalar_field)
{
}

Isosurface::~Isosurface() {}

void
Isosurface::CalculateGradient()
{
  int const xsize = dimensions_.x;
  int const ysize = dimensions_.y;
  int const zsize = dimensions_.z;

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
          x_diff = CentralDifference(Value(x - 1, y, z), Value(x + 1, y, z));
        }
        if (y == 0) {
          y_diff = ForwardDifference(Value(x, 0, z), Value(x, 1, z));
        } else if (y == ysize - 1) {
          y_diff = BackwardDifference(Value(x, y, z), Value(x, y - 1, z));
        } else {
          y_diff = CentralDifference(Value(x, y - 1, z), Value(x, y + 1, z));
        }
        if (z == 0) {
          z_diff = ForwardDifference(Value(x, y, 0), Value(x, y, 1));
        } else if (z == zsize - 1) {
          z_diff = BackwardDifference(Value(x, y, z), Value(x, y, z - 1));
        } else {
          z_diff = CentralDifference(Value(x, y, z - 1), Value(x, y, z + 1));
        }

        if (x_diff == 0 && y_diff == 0 && z_diff == 0) {
          gradients_.emplace_back(0.f, 0.f, 0.f);
        } else {
          gradients_.push_back(glm::normalize(glm::vec3{x_diff, y_diff, z_diff}));
        }
      }
    }
  }
}

std::pair<unsigned int, std::vector<float>>
Isosurface::MarchingCube()
{
  CalculateGradient();

  std::vector<float> data;
  unsigned int vertex_count = 0;

  int const xsize = dimensions_.x;
  int const ysize = dimensions_.y;
  int const zsize = dimensions_.z;
  GridCell cell;

  constexpr std::array<short, 8> cmp_results{1, 2, 4, 8, 16, 32, 64, 128};

  for (int z = 0; z < zsize - 1; ++z) {
    for (int y = 0; y < ysize - 1; ++y) {
      for (int x = 0; x < xsize - 1; ++x) {

        unsigned short vertex_cmp_result = 0;
        cell.SetBaseIndex(x, y, z);

        for (int i = 0; i <= 7; ++i) {
          if (Value(cell[i]) < isovalue_)
            vertex_cmp_result |= cmp_results[i];
        }

        auto const intersected_edges = table::kEdgeTable[vertex_cmp_result];

        if (intersected_edges == 0)
          continue;

        std::array<std::array<float, 6>, 12> edge_list;
        if (intersected_edges & 1)
          edge_list[0] = InterpVertexAttribs(cell[0], cell[1]);
        if (intersected_edges & 2)
          edge_list[1] = InterpVertexAttribs(cell[1], cell[2]);
        if (intersected_edges & 4)
          edge_list[2] = InterpVertexAttribs(cell[2], cell[3]);
        if (intersected_edges & 8)
          edge_list[3] = InterpVertexAttribs(cell[3], cell[0]);
        if (intersected_edges & 16)
          edge_list[4] = InterpVertexAttribs(cell[4], cell[5]);
        if (intersected_edges & 32)
          edge_list[5] = InterpVertexAttribs(cell[5], cell[6]);
        if (intersected_edges & 64)
          edge_list[6] = InterpVertexAttribs(cell[6], cell[7]);
        if (intersected_edges & 128)
          edge_list[7] = InterpVertexAttribs(cell[7], cell[4]);
        if (intersected_edges & 256)
          edge_list[8] = InterpVertexAttribs(cell[0], cell[4]);
        if (intersected_edges & 512)
          edge_list[9] = InterpVertexAttribs(cell[1], cell[5]);
        if (intersected_edges & 1024)
          edge_list[10] = InterpVertexAttribs(cell[2], cell[6]);
        if (intersected_edges & 2048)
          edge_list[11] = InterpVertexAttribs(cell[3], cell[7]);

        for (int i = 0; table::kTriTable[vertex_cmp_result][i] != -1; ++i) {
          auto const edge_index = table::kTriTable[vertex_cmp_result][i];
          std::array<float, 6> const& attrbs = edge_list[edge_index];
          data.insert(data.end(), attrbs.begin(), attrbs.end());
          vertex_count += 3;
        }
      }
    }
  }
  data.shrink_to_fit();
  return std::make_pair(vertex_count, data);
}

std::array<float, 6>
Isosurface::InterpVertexAttribs(glm::vec3 v1, glm::vec3 v2)
{
  constexpr float min = 0.0001;
  float const value1 = Value(v1);
  float const value2 = Value(v2);
  float const ratio = (isovalue_ - value1) / (value2 - value1);

  glm::vec3 const& g1 = Gradient(v1);
  glm::vec3 const& g2 = Gradient(v2);
  glm::vec3 const p1 = v1 * model_ratio_;
  glm::vec3 const p2 = v2 * model_ratio_;

  if (std::abs(value1 - value2) < min) {
    glm::vec3 const v = (p1 + p2) * 0.5f;
    glm::vec3 const g = glm::normalize((g1 + g2) * 0.5f);
    return std::array<float, 6>{v.x, v.y, v.z, g.x, g.y, g.z};
  }
  if (std::abs(isovalue_ - value1) < min) {
    return std::array<float, 6>{p1.x, p1.y, p1.z, g1.x, g1.y, g1.z};
  }
  if (std::abs(isovalue_ - value2) < min) {
    return std::array<float, 6>{p2.x, p2.y, p2.z, g2.x, g2.y, g2.z};
  }

  glm::vec3 const g = glm::normalize(ratio * (g2 - g1) + g1);
  glm::vec3 const v = ratio * (p2 - p1) + p1;
  return std::array<float, 6>{v.x, v.y, v.z, g.x, g.y, g.z};
}

inline float
Isosurface::Value(int x, int y, int z) const
{
  return scalar_field_[Index(x, y, z)];
}

inline float
Isosurface::Value(glm::vec3 const& v) const
{
  return Value(v.x, v.y, v.z);
}

inline glm::vec3 const&
Isosurface::Gradient(int x, int y, int z) const
{
  return gradients_[Index(x, y, z)];
}

inline glm::vec3 const&
Isosurface::Gradient(glm::vec3 const& v) const
{
  return Gradient(v.x, v.y, v.z);
}

inline int
Isosurface::Index(int x, int y, int z) const
{
  int const xsize = dimensions_.x;
  int const ysize = dimensions_.y;
  return x + y * xsize + z * ysize * xsize;
}

inline float
Isosurface::CentralDifference(float front, float back) const
{
  return (back - front) / 2;
}

inline float
Isosurface::BackwardDifference(float self, float back) const
{
  return self - back;
}

inline float
Isosurface::ForwardDifference(float self, float front) const
{
  return BackwardDifference(front, self);
}

void
Isosurface::SetIsovalue(float value)
{
  isovalue_ = value;
}

void
Isosurface::SetModelDimensions(glm::ivec3 dimensions)
{
  dimensions_ = dimensions;
}

void
Isosurface::SetModelRatio(glm::vec3 ratio)
{
  model_ratio_ = ratio;
}

Isosurface::GridCell::GridCell()
  : x_(0)
  , y_(0)
  , z_(0)
{
}

Isosurface::GridCell::~GridCell(){};

void
Isosurface::GridCell::SetBaseIndex(int x, int y, int z)
{
  x_ = x;
  y_ = y;
  z_ = z;
}

glm::vec3 Isosurface::GridCell::operator[](int index) const
{
  using namespace glm;

  switch (index) {
  case 0:
    return vec3{x_, y_, z_};
  case 1:
    return vec3{x_ + 1, y_, z_};
  case 2:
    return vec3{x_ + 1, y_, z_ + 1};
  case 3:
    return vec3{x_, y_, z_ + 1};
  case 4:
    return vec3{x_, y_ + 1, z_};
  case 5:
    return vec3{x_ + 1, y_ + 1, z_};
  case 6:
    return vec3{x_ + 1, y_ + 1, z_ + 1};
  case 7:
    return vec3{x_, y_ + 1, z_ + 1};
  }
  fprintf(stderr, "In Isosurface::GridCell: Wrong index.");
  return vec3{-1, -1, -1};
}
