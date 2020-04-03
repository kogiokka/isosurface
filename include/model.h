#pragma once

#include <array>
#include <cassert>
#include <cstdio>
#include <string_view>
#include <utility>
#include <vector>

class Model
{
  unsigned int vertex_count_;
  std::array<int, 3> dimensions_;
  std::vector<float> render_data_;

public:
  Model();
  ~Model();
  Model(Model const&) = delete;
  Model& operator=(Model const&) = delete;
  void SetRenderData(std::pair<unsigned int, std::vector<float>> data);
  float const* RenderData() const;
  unsigned int VertexCount() const;
};
