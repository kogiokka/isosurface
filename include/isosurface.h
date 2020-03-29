#pragma once

#include <vector>
#include <string_view>
#include <sstream>
#include <fstream>
#include <filesystem>

#include "glm/glm.hpp"

class Isosurface
{
  std::vector<int> dimensions_;
  std::vector<char> voxels_;
  std::vector<glm::vec3> voxel_gradient_;

public:
  Isosurface();
  ~Isosurface();
  void ReadRawInfo(std::string_view const filepath);
  void ReadRaw(std::string_view const filepath);
};
