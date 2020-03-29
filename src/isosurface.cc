#include "isosurface.h"

Isosurface::Isosurface() {}

Isosurface::~Isosurface() {}

void
Isosurface::ReadRaw(std::string_view const filepath)
{
  assert(!dimensions_.empty());

  std::ifstream file(filepath.data(), std::ios::binary);
  if (file.fail()) {
    fprintf(stderr, "Failed to read raw file.\n");
    exit(EXIT_FAILURE);
  }

  auto file_size = std::filesystem::file_size("data/test.raw");
  voxels_.resize(file_size);
  file.read(voxels_.data(), file_size);
}

void
Isosurface::ReadRawInfo(std::string_view const filepath)
{
  std::ifstream file(filepath.data(), std::ios::binary);
  if (file.fail()) {
    fprintf(stderr, "Failed to read raw file.\n");
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

  std::stringstream ss(attrs.at(0));
  std::string dimen;
  while (std::getline(ss, dimen, ':')) {
    dimensions_.push_back(std::stoi(dimen));
  }
}
