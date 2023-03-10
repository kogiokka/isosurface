#include "VolumeData.hpp"
#include "Util.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <utility>

typedef struct KeyAliases {
  std::vector<std::string> static const Raw;
  std::vector<std::string> static const Rez;
  std::vector<std::string> static const VSize;
  std::vector<std::string> static const Type;
  std::vector<std::string> static const Endian;
} KeyAliases;

typedef struct InfKey {
  std::string static const Inf;
  std::string static const Raw;
  std::string static const Rez;
  std::string static const VSize;
  std::string static const Type;
  std::string static const Endian;
} InfKey;

std::vector<std::string> const KeyAliases::Raw{"raw-file", "RawFile"};
std::vector<std::string> const KeyAliases::Rez{"resolution", "Resolution"};
std::vector<std::string> const KeyAliases::VSize{"voxel-size", "VoxelSize", "ratio", "Ratio"};
std::vector<std::string> const KeyAliases::Type{"sample-type", "SampleType"};
std::vector<std::string> const KeyAliases::Endian{"endian", "Endian"};
std::string const InfKey::Inf = "inf-file";
std::string const InfKey::Raw = "raw-file";
std::string const InfKey::Rez = "resolution";
std::string const InfKey::VSize = "voxel-size";
std::string const InfKey::Type = "sample-type";
std::string const InfKey::Endian = "endian";

KeyAliases static const KEY_ALIAS;
InfKey static const KEY;

VolumeData::VolumeData(std::string const& infFile)
  : info_(0)
  , resolution_()
  , ratio_()
  , scalarField_(0)
  , dataType_(DataType::UnsignedChar)
{
  info_.emplace(KEY.Inf, infFile);

  using namespace std;
  namespace fs = std::filesystem;

  // Read INF file
  ifstream infFh(info_.at(KEY.Inf));
  if (infFh.fail()) {
    cerr << "Failed to read INF file: \"" << info_.at("inf") << "\"\n";
    exit(EXIT_FAILURE);
  }

  string line;
  while (std::getline(infFh, line)) {
    string::size_type n = line.find("=");
    if (n == string::npos)
      continue;
    auto value = util::str::TrimCR(line.substr(n + 1));
    if (value.empty())
      continue;
    auto key = line.substr(0, n);

    // Find out the keys used in the INF file.
    // In case the INF file format is not standardized, we hard-code every possibility into "keyList"s and search
    // through it. NOTE: Do not make the path string lowercase, OS like Linux and BSD are case-sensitive.
    if (find(KEY_ALIAS.Rez.begin(), KEY_ALIAS.Rez.end(), key) != KEY_ALIAS.Rez.end()) {
      info_.emplace(KEY.Rez, move(value));
    } else if (find(KEY_ALIAS.VSize.begin(), KEY_ALIAS.VSize.end(), key) != KEY_ALIAS.VSize.end()) {
      info_.emplace(KEY.VSize, move(value));
    } else if (find(KEY_ALIAS.Type.begin(), KEY_ALIAS.Type.end(), key) != KEY_ALIAS.Type.end()) {
      // Make the value string lowercase for easier comparison.
      transform(value.cbegin(), value.cend(), value.begin(), ::tolower);
      info_.emplace(KEY.Type, move(value));
    } else if (find(KEY_ALIAS.Endian.begin(), KEY_ALIAS.Endian.end(), key) != KEY_ALIAS.Endian.end()) {
      // Make the value string lowercase for easier comparison.
      transform(value.cbegin(), value.cend(), value.begin(), ::tolower);
      info_.emplace(KEY.Endian, move(value));
    } else if (find(KEY_ALIAS.Raw.begin(), KEY_ALIAS.Raw.end(), key) != KEY_ALIAS.Raw.end()) {
      info_.emplace(KEY.Raw, fs::path(info_.at(KEY.Inf)).replace_filename(value).string());
    }
  }

  if (info_.find(KEY.Rez) == info_.end()) {
    std::cerr << "[ERROR] .inf file: Missing attribute \"resolution\"\n";
    exit(EXIT_FAILURE);
  }
  if (info_.find(KEY.Type) == info_.end()) {
    std::cerr << "[ERROR] .inf file: Missing attribute \"sample-type\"\n";
    exit(EXIT_FAILURE);
  }

  // Give the required attributes default values if the key does not exist.
  if (info_.find(KEY.VSize) == info_.end()) {
    info_.emplace(KEY.VSize, "1:1:1");
  }
  if (info_.find(KEY.Endian) == info_.end()) {
    info_.emplace(KEY.Endian, "little");
  }
  if (info_.find(KEY.Raw) == info_.end()) {
    info_.emplace(KEY.Raw, fs::path(info_.at(KEY.Inf)).replace_extension(".raw").string());
  }
}

VolumeData::~VolumeData() {}

void
VolumeData::SetRawFile(const std::string& rawFile)
{
  namespace fs = std::filesystem;
  if (!fs::exists(rawFile)) {
    std::cerr << "Non-existent file: \"" << rawFile << "\"\n";
    std::exit(EXIT_FAILURE);
  }
  info_[KEY.Raw] = rawFile;
}

bool
VolumeData::Read()
{
  using namespace std;
  namespace fs = std::filesystem;

  if (!scalarField_.empty())
    return false;

  // Split by ":", "x", or "X". Ex. 0.1:0.1:0.1, 149x208x110
  vector<std::string> const rez = util::str::Split(info_[KEY.Rez], "[:xX]");
  vector<std::string> const ratio = util::str::Split(info_[KEY.VSize], "[:xX]");
  string const& type = info_[KEY.Type];
  for (std::size_t i = 0; i < 3; ++i) {
    resolution_[i] = stoul(rez[i]);
    ratio_[i] = stof(ratio[i]);
  }

  if (type == "unsigned_char" || type == "unsignedchar") {
    dataType_ = DataType::UnsignedChar;
  } else if (type == "unsigned_short" || type == "unsignedshort") {
    dataType_ = DataType::UnsignedShort;
  } else if (type == "float") {
    dataType_ = DataType::Float;
  } else if (type == "short") {
    dataType_ = DataType::Short;
  } else {
    cerr << "Unknown model data type: \"" << type.c_str() << "\"\n";
    return false;
  }

  // Read RAW file
  ifstream rawFile(info_.at(KEY.Raw), ios::binary);
  if (rawFile.fail()) {
    cerr << "Failed to read RAW file: \"" << info_.at(KEY.Raw) << "\"\n";
    exit(EXIT_FAILURE);
  }

  auto file_size = fs::file_size(info_.at(KEY.Raw));

  switch (dataType_) {
  case DataType::UnsignedChar: {
    vector<uint8_t> buffer(file_size);
    rawFile.read(reinterpret_cast<char*>(buffer.data()), file_size);
    scalarField_.assign(buffer.begin(), buffer.end());
  } break;
  case DataType::UnsignedShort: {
    vector<uint16_t> buffer(file_size / sizeof(uint16_t));
    rawFile.read(reinterpret_cast<char*>(buffer.data()), file_size);
    if (info_.at(KEY.Endian) == "big") {
      for (auto& n : buffer) {
        n = util::endian::BigToLittle<unsigned short>(n);
      }
    }
    scalarField_.assign(buffer.begin(), buffer.end());
  } break;
  case DataType::Float: {
    vector<float> buffer(file_size / sizeof(float));
    rawFile.read(reinterpret_cast<char*>(buffer.data()), file_size);
    if (info_.at(KEY.Endian) == "big") {
      for (auto& n : buffer) {
        n = util::endian::BigToLittle<float>(n);
      }
    }
    scalarField_.assign(buffer.begin(), buffer.end());
  } break;
  case DataType::Short: {
    vector<int16_t> buffer(file_size / sizeof(int16_t));
    rawFile.read(reinterpret_cast<char*>(buffer.data()), file_size);
    if (info_.at(KEY.Endian) == "big") {
      for (auto& n : buffer) {
        n = util::endian::BigToLittle<short>(n);
      }
    }
    scalarField_.assign(buffer.begin(), buffer.end());
  } break;
  }

  rawFile.close();

  return true;
}

bool
VolumeData::Write(std::string const& dirPath, std::string const& name) const
{
  assert(!scalarField_.empty());

  using namespace std;

  auto const pathInf = dirPath + "/" + name + ".inf";
  auto const pathRaw = dirPath + "/" + name + ".raw";

  // TODO: INF file

  ofstream rawFile(pathRaw, std::ios::binary);
  if (rawFile.fail())
    return false;
  rawFile.write(reinterpret_cast<char const*>(scalarField_.data()), scalarField_.size() * sizeof(float));
  rawFile.close();

  return true;
}

glm::ivec3
VolumeData::Resolution() const
{
  return resolution_;
}

glm::vec3
VolumeData::Ratio() const
{
  return ratio_;
}

glm::vec3
VolumeData::Dimensions() const
{
  return {
    resolution_[0] * ratio_[0],
    resolution_[1] * ratio_[1],
    resolution_[2] * ratio_[2],
  };
}

std::string
VolumeData::InfFile() const
{
  return info_.at(KEY.Inf);
}

std::string
VolumeData::RawFile() const
{
  return info_.at(KEY.Raw);
}

std::vector<float>&
VolumeData::Data()
{
  return scalarField_;
}

std::vector<float> const&
VolumeData::Data() const
{
  return scalarField_;
}

float
VolumeData::Min() const
{
  return *std::min_element(scalarField_.begin(), scalarField_.end());
}

float
VolumeData::Max() const
{
  return *std::max_element(scalarField_.begin(), scalarField_.end());
}

bool
VolumeData::Loaded() const
{
  return !scalarField_.empty();
}
