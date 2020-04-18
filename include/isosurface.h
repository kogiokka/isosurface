#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <utility>
#include <vector>

#include "constants.h"
#include "glm/glm.hpp"

class Isosurface
{
  unsigned int isovalue_;
  glm::ivec3 dimensions_;
  glm::vec3 model_ratio_;
  std::vector<glm::vec3> gradients_;
  std::vector<float> scalar_field_;

  class GridCell
  {
    int x_;
    int y_;
    int z_;
    std::array<glm::vec3, 8> voxel_index_;

  public:
    GridCell(int x, int y, int z);
    ~GridCell();
    glm::vec3 const& operator[](int index) const;
  };

public:
  Isosurface(std::vector<float> scalar_field);
  ~Isosurface();
  void SetIsovalue(float value);
  void SetModelDimensions(glm::ivec3 dimensions);
  void SetModelRatio(glm::vec3 ratio);
  std::pair<unsigned int, std::vector<float>> MarchingCube();

private:
  std::array<float, 6> InterpVertexAttribs(glm::vec3 const& v1, glm::vec3 const& v2);
  inline void CalculateGradient();
  inline float CenteredDifference(float front, float back) const;
  inline float ForwardDifference(float self, float front) const;
  inline float BackwardDifference(float self, float back) const;
  inline float Value(int x, int y, int z) const;
  inline float Value(glm::vec3 const& v) const;
  inline glm::vec3 const& Gradient(int x, int y, int z) const;
  inline int Index(int x, int y, int z) const;
};
