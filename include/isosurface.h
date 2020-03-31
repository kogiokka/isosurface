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
  unsigned int target_value_;
  unsigned int vertex_count_;
  std::array<int, 3> dimensions_;
  std::vector<unsigned char> isovalues_;
  std::vector<glm::vec3> gradients_;
  std::vector<float> render_data_;

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
  Isosurface();
  ~Isosurface();
  void ReadRawInfo(std::string_view const filepath);
  void ReadRaw(std::string_view const filepath);
  void CalculateGradient();
  void MarchingCube();
  float const* RenderData() const;
  unsigned int RenderVertexCount() const;

private:
  glm::vec3 InterpolatedVertex(glm::vec3 const& v1, glm::vec3 const& v2);
  glm::vec3 InterpolatedNormal(glm::vec3 const& v1, glm::vec3 const& v2);

private:
  inline float CenteredDifference(unsigned short front,
                                  unsigned short back) const;
  inline float ForwardDifference(unsigned short self,
                                 unsigned short front) const;
  inline float BackwardDifference(unsigned short self,
                                  unsigned short back) const;
  inline unsigned short Value(int x, int y, int z) const;
  inline unsigned short Value(glm::vec3 const& v) const;
  inline glm::vec3 const& Gradient(int x, int y, int z) const;
  inline int Index(int x, int y, int z) const;
};
