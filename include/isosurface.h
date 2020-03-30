#pragma once

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string_view>
#include <vector>

#include "glm/glm.hpp"

#include "constants.h"

class Isosurface
{
  float const h_;
  unsigned int target_value_;
  std::vector<int> dimensions_;
  std::vector<unsigned char> isovalues_;
  std::vector<glm::vec3> gradients_;
  std::vector<glm::vec3> vertices_;
  std::vector<glm::vec3> normals_;

  class GridCell
  {
    int x_;
    int y_;
    int z_;
    std::vector<glm::vec3> voxel_index_;

  public:
    GridCell(int x, int y, int z);
    ~GridCell();
    glm::vec3 const& VoxelIndex(int index) const;
  };

public:
  Isosurface();
  ~Isosurface();
  void ReadRawInfo(std::string_view const filepath);
  void ReadRaw(std::string_view const filepath);
  void CalculateGradient();
  void MarchingCube();
  glm::vec3 InterpolatedVertex(glm::vec3 idx1, glm::vec3 idx2);
  glm::vec3 InterpolatedNormal(glm::vec3 idx1, glm::vec3 idx2);

private:
  float CenteredDifference(unsigned short front, unsigned short back) const;
  float ForwardDifference(unsigned short self, unsigned short front) const;
  float BackwardDifference(unsigned short self, unsigned short back) const;

private:
  inline unsigned short Value(int x, int y, int z) const;
  inline glm::vec3 const& Gradient(int x, int y, int z) const;
  inline int Index(int x, int y, int z) const;
};
