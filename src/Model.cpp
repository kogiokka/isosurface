#include "Model.hpp"
#include <cassert>
#include <cstdio>

Model::Model(std::string const& infPath)
  : vertexCount_(0)
  , renderData_(0)
  , volumeData(infPath)
{
  volumeData.Read();
}

Model::~Model() {}

void
Model::SetRenderData(std::pair<unsigned int, std::vector<float>> data)
{
  std::tie(vertexCount_, renderData_) = data;
}

float const*
Model::RenderData() const
{
  return renderData_.data();
}

unsigned int
Model::VertexCount() const
{
  return vertexCount_;
}

void
Model::GenIsosurface(float isovalue, int method)
{
  assert(!volumeData.Data().empty());
  Isosurface isosurface(volumeData.Data());
  isosurface.SetIsovalue(isovalue);
  isosurface.SetResolution(volumeData.Resolution());
  isosurface.SetRatio(volumeData.Ratio());
  switch (method) {
  case 1:
    SetRenderData(isosurface.MarchingTetrahedra());
    break;
  default:
    SetRenderData(isosurface.MarchingCubes());
    break;
  }
}

glm::vec3
Model::Center() const
{
  return volumeData.Dimensions() * 0.5f;
}
