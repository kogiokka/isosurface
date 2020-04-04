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

  std::string type = attrs.at(2);
  if (type.back() == '\r')
    type.pop_back();
  if (type == "unsigned_char") {
    data_type_ = DataType::kUnsignedChar;
  } else if (type == "unsigned_short") {
    data_type_ = DataType::kUnsignedShort;
  } else {
    std::fprintf(stderr, "Unknown model data type: %s\n", type.c_str());
    exit(EXIT_FAILURE);
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

  std::vector<float> field;
  switch (data_type_) {
  case DataType::kUnsignedChar:
    field.insert(begin(field), buf.data(), buf.data() + file_size);
    break;
  case DataType::kUnsignedShort:;
    field.insert(begin(field),
                 reinterpret_cast<unsigned short*>(buf.data()),
                 reinterpret_cast<unsigned short*>(buf.data()) + static_cast<int>(0.5f * file_size));
    break;
  }
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
