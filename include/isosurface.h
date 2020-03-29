#pragma once

#include <algorithm>
#include <vector>
#include <string_view>
#include <sstream>
#include <fstream>
#include <filesystem>

#include "glm/glm.hpp"

class Isosurface
{
  float const h;
  unsigned int target_value_;
  std::vector<int> dimensions_;
  std::vector<unsigned char> isovalues_;
  std::vector<glm::vec3> gradients_;

public:
  Isosurface();
  ~Isosurface();
  void ReadRawInfo(std::string_view const filepath);
  void ReadRaw(std::string_view const filepath);
  void Interpolate();
  void CalculateGradient();

private:
  float CenteredDifference(unsigned short front, unsigned short back) const;
  float ForwardDifference(unsigned short self, unsigned short front) const;
  float BackwardDifference(unsigned short self, unsigned short back) const;

private:
  inline unsigned short Value(int x, int y, int z) const;
  inline int Index(int x, int y, int z) const;
};
