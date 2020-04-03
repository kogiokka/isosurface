#include "model.h"

Model::Model(std::string const inf, std::string const raw)
  : inf_file_(inf)
  , raw_file_(raw)
  , vertex_count_(0)
  , dimensions_{ 0, 0, 0 }
  , render_data_(0)
{
  std::ifstream file(inf_file_, std::ios::binary);
  if (file.fail()) {
    std::fprintf(stderr, "Failed to read raw file.\n");
    exit(EXIT_FAILURE);
  }
  std::string value;
  std::vector<std::string> attrs;
  while (std::getline(file, value)) {
    std::string::size_type n = value.find("=");
    if (n == std::string::npos)
      continue;
    attrs.push_back(value.substr(n + 1));
  }

  std::string token;
  std::istringstream attr_dimen(attrs.at(0));
  std::istringstream attr_ratio(attrs.at(1));

  int index = 0;
  while (std::getline(attr_dimen, token, ':')) {
    dimensions_[index] = std::stoi(token);
    ++index;
  }

  index = 0;
  while (std::getline(attr_ratio, token, ':')) {
    ratio_[index] = std::stof(token);
    ++index;
  }
};

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

std::array<int, 3>
Model::Dimension()
{
  return dimensions_;
}

std::array<float, 3>
Model::Ratio()
{
  return ratio_;
}

std::vector<float>
Model::ScalarField()
{
  assert(dimensions_[0] > 0);
  assert(dimensions_[1] > 0);
  assert(dimensions_[2] > 0);

  std::ifstream file(raw_file_, std::ios::binary);
  if (file.fail()) {
    fprintf(stderr, "Failed to read raw file.\n");
    exit(EXIT_FAILURE);
  }

  auto file_size = std::filesystem::file_size(raw_file_);

  std::vector<unsigned char> buf(file_size);
  file.read(reinterpret_cast<char*>(buf.data()), file_size);
  file.close();

  // int size = buf.size() * 0.5;
  // unsigned short* tmp;
  // tmp = static_cast<unsigned short*>(static_cast<void*>(buf.data()));
  // std::vector<float> field;
  // std::copy(tmp, tmp + size, std::back_inserter(field));
  std::vector<float> field;
  field.reserve(buf.size() * 4);
  for (auto const& v : buf)
    field.push_back(static_cast<float>(v));
  return field;
}

std::array<int, 3>
Model::Center() const
{
  std::array<int, 3> center;
  for (int i = 0; i < 3; ++i)
    center[i] = dimensions_[i] * 0.5f;

  return center;
}
