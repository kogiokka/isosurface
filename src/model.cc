#include "model.h"

Model::Model()
  : vertex_count_(0)
  , dimensions_{ 0, 0, 0 }
  , render_data_(0){};

Model::~Model(){};

void
Model::SetRenderData(std::pair<unsigned int, std::vector<float>> data)
{
  vertex_count_ = std::get<0>(data);
  render_data_ = std::get<1>(data);
}

float const*
Model::RenderData() const
{
  return render_data_.data();
}

unsigned int
Model::VertexCount() const
{
  return vertex_count_;
}
