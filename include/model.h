#pragma once

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "glm/glm.hpp"
#include "isosurface.h"

class Model
{
  enum class DataType { kUnsignedChar, kUnsignedShort };
  unsigned int id_;
  unsigned int vertex_count_;
  DataType data_type_;
  glm::ivec3 dimensions_;
  glm::vec3 ratio_;
  std::string inf_file_;
  std::string raw_file_;
  std::vector<float> render_data_;

public:
  Model(std::filesystem::path const inf_path, std::filesystem::path const raw_path);
  ~Model();
  Model(Model const&) = delete;
  Model& operator=(Model const&) = delete;

  std::vector<float> ScalarField();
  glm::vec3 Center() const;
  float const* RenderData() const;
  unsigned int VertexCount() const;
  void SetRenderData(std::pair<unsigned int, std::vector<float>> data);
  void GenIsosurface(float isovalue);
  unsigned int& Id();
  unsigned int Id() const;
};
