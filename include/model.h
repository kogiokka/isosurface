#pragma once

#include <array>
#include <cassert>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

class Model
{
  enum class DataType { kUnsignedChar, kUnsignedShort };
  std::string inf_file_;
  std::string raw_file_;
  DataType data_type_;
  unsigned int vertex_count_;
  std::array<int, 3> dimensions_;
  std::array<float, 3> ratio_;
  std::vector<float> render_data_;


public:
  Model(std::string const inf, std::string const raw);
  ~Model();
  std::array<int, 3> Dimension();
  std::array<float, 3> Ratio();
  std::vector<float> ScalarField();
  std::array<int, 3> Center() const;
  void SetRenderData(std::pair<unsigned int, std::vector<float>> data);
  float const* RenderData() const;
  unsigned int VertexCount() const;
  Model(Model const&) = delete;
  Model& operator=(Model const&) = delete;
};