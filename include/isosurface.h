#pragma once

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string_view>
#include <utility>
#include <vector>

#include "glm/glm.hpp"

#include "constants.h"

class Isosurface
{
  unsigned int isovalue_;
  std::array<int, 3> dimensions_;
  std::vector<glm::vec3> gradients_;
  std::vector<unsigned char> scalar_field_;

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
  Isosurface(float isovalue);
  ~Isosurface();
  void CalculateGradient();
  std::pair<unsigned int, std::vector<float>> MarchingCube();
  void ReadInfo(std::string_view const filepath);
  void ReadRaw(std::string_view const filepath);

private:
  std::array<float, 6> InterpVertexAttribs(glm::vec3 const& v1, glm::vec3 const& v2);
  inline float CenteredDifference(unsigned short front, unsigned short back) const;
  inline float ForwardDifference(unsigned short self, unsigned short front) const;
  inline float BackwardDifference(unsigned short self, unsigned short back) const;
  inline unsigned short Value(int x, int y, int z) const;
  inline unsigned short Value(glm::vec3 const& v) const;
  inline glm::vec3 const& Gradient(int x, int y, int z) const;
  inline int Index(int x, int y, int z) const;
};
