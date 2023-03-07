#pragma once

#include "Isosurface.hpp"
#include "VolumeData.hpp"

#include "glm/glm.hpp"

#include <string>
#include <utility>
#include <vector>

class Model
{
  unsigned int vertexCount_;
  std::vector<float> renderData_;
  VolumeData volumeData;

public:
  Model(std::string const& infPath);
  ~Model();
  Model(Model const&) = delete;
  Model& operator=(Model const&) = delete;

  glm::vec3 Center() const;
  float const* RenderData() const;
  unsigned int VertexCount() const;
  void SetRenderData(std::pair<unsigned int, std::vector<float>> data);
  void GenIsosurface(float isovalue, int method);
};
