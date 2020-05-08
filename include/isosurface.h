#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <utility>
#include <vector>
#include <bitset>
#include <iostream>

#include "marching_cubes_tables.h"
#include "marching_tetrahedra_tables.h"
#include "glm/glm.hpp"

class Isosurface
{
  unsigned int isovalue_;
  glm::ivec3 dimensions_;
  glm::vec3 model_ratio_;
  std::vector<glm::vec3> gradients_;
  std::vector<float> scalar_field_;
  class GridCell;

public:
  Isosurface(std::vector<float> scalar_field);
  ~Isosurface();
  void SetIsovalue(float value);
  void SetModelDimensions(glm::ivec3 dimensions);
  void SetModelRatio(glm::vec3 ratio);
  std::pair<unsigned int, std::vector<float>> MarchingCubes();
  std::pair<unsigned int, std::vector<float>> MarchingTetrahedra();

private:
  void CalculateGradient();
  std::array<short, 6> TetrahedraStatus(short cube_status) const;
  std::array<float, 6> InterpVertexAttribs(glm::vec3 v1, glm::vec3 v2) const;
  float CentralDifference(float front, float back) const;
  float ForwardDifference(float self, float front) const;
  float BackwardDifference(float self, float back) const;
  float Value(int x, int y, int z) const;
  float Value(glm::vec3 const& v) const;
  int Index(int x, int y, int z) const;
  glm::vec3 const& Gradient(int x, int y, int z) const;
  glm::vec3 const& Gradient(glm::vec3 const& v) const;
};

class Isosurface::GridCell
{
  int x_;
  int y_;
  int z_;

public:
  GridCell();
  ~GridCell();
  void SetBaseIndex(int x, int y, int z);
  glm::vec3 operator[](int index) const;
};
