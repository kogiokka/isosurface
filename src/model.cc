#include "model.h"

Model::Model(std::filesystem::path const inf_path, std::filesystem::path const raw_path)
  : id_(0)
  , vertex_count_(0)
  , dimensions_{ 0, 0, 0 }
  , inf_file_(inf_path)
  , raw_file_(raw_path)
  , render_data_(0)
{
  std::ifstream file(inf_file_, std::ios::binary);
  if (file.fail()) {
    std::fprintf(stderr, "Failed to read raw file: %s\n", raw_path.c_str());
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

  std::vector<int> tmp_dimen;
  std::vector<float> tmp_ratio;
  while (std::getline(attr_dimen, token, ':')) {
    tmp_dimen.emplace_back(std::stoi(token));
  }
  while (std::getline(attr_ratio, token, ':')) {
    tmp_ratio.emplace_back(std::stof(token));
  }
  dimensions_ = { tmp_dimen[0], tmp_dimen[1], tmp_dimen[2] };
  ratio_ = { tmp_ratio[0], tmp_ratio[1], tmp_ratio[2] };

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

void
Model::GenIsosurface(float isovalue)
{
  Isosurface isosurface(ScalarField());
  isosurface.SetIsovalue(isovalue);
  isosurface.SetModelDimensions(dimensions_);
  isosurface.SetModelRatio(ratio_);
  SetRenderData(isosurface.MarchingCube());
}

std::vector<float>
Model::ScalarField()
{
  assert(dimensions_.x > 0);
  assert(dimensions_.y > 0);
  assert(dimensions_.z > 0);

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

glm::vec3
Model::Center() const
{
  return glm::vec3{ dimensions_ } * 0.5f;
}

unsigned int
Model::Id() const
{
  return id_;
}

unsigned int&
Model::Id()
{
  return id_;
}
