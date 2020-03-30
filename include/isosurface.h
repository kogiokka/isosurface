#pragma once

#include <algorithm>
#include <vector>
#include <string_view>
#include <sstream>
#include <fstream>
#include <filesystem>

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
  glm::vec3 EdgeIndex(int index, int x, int y, int z) const;

private:
  inline unsigned short Value(int x, int y, int z) const;
  inline glm::vec3 const& Gradient(int x, int y, int z) const;
  inline int Index(int x, int y, int z) const;
};
