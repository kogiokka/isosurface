#include "isosurface.h"

Isosurface::Isosurface()
  : h(1.f)
{}

Isosurface::~Isosurface() {}

void
Isosurface::CalculateGradient()
{
  int const xsize = dimensions_[0];
  int const ysize = dimensions_[1];
  int const zsize = dimensions_[2];

  for (int z = 0; z < zsize; ++z) {
    for (int y = 0; y < ysize; ++y) {
      for (int x = 0; x < xsize; ++x) {
        float x_diff;
        float y_diff;
        float z_diff;
        if (x == 0) {
          x_diff = ForwardDifference(Value(0, y, z), Value(1, y, z));
        } else if (x == xsize - 1) {
          x_diff = BackwardDifference(Value(x - 1, y, z), Value(x, y, z));
        } else {
          x_diff = CenteredDifference(Value(x - 1, y, z), Value(x + 1, y, z));
        }
        if (y == 0) {
          y_diff = ForwardDifference(Value(x, 0, z), Value(x, 1, z));
        } else if (y == ysize - 1) {
          y_diff = BackwardDifference(Value(x, y - 1, z), Value(x, y, z));
        } else {
          y_diff = CenteredDifference(Value(x, y - 1, z), Value(x, y + 1, z));
        }
        if (z == 0) {
          z_diff = ForwardDifference(Value(x, y, 0), Value(x, y, 1));
        } else if (z == zsize - 1) {
          z_diff = BackwardDifference(Value(x, y, z - 1), Value(x, y, z));
        } else {
          z_diff = CenteredDifference(Value(x, y, z - 1), Value(x, y, z + 1));
        }

        gradients_.emplace_back(x_diff, y_diff, z_diff);
      }
    }
  }
}

void
Isosurface::Interpolate()
{}

inline unsigned short
Isosurface::Value(int x, int y, int z) const
{
  return static_cast<unsigned short>(isovalues_.at(Index(x, y, z)));
}

inline int
Isosurface::Index(int x, int y, int z) const
{
  int const xsize = dimensions_[0];
  int const ysize = dimensions_[1];

  return x + y * xsize + z * ysize * xsize;
}

float
Isosurface::CenteredDifference(unsigned short front, unsigned short back) const
{
  return static_cast<float>(back - front) / 2 * h;
}

float
Isosurface::BackwardDifference(unsigned short self, unsigned short back) const
{
  return static_cast<float>(back - self) / h;
}

float
Isosurface::ForwardDifference(unsigned short self, unsigned short front) const
{
  return BackwardDifference(front, self);
}

void
Isosurface::ReadRaw(std::string_view const filepath)
{
  assert(!dimensions_.empty());

  std::ifstream file(filepath.data(), std::ios::binary);
  if (file.fail()) {
    fprintf(stderr, "Failed to read raw file.\n");
    exit(EXIT_FAILURE);
  }

  auto file_size = std::filesystem::file_size(filepath);

  std::vector<char> tmp(file_size);
  file.read(tmp.data(), file_size);
  file.close();

  std::move(tmp.begin(), tmp.end(), std::back_inserter(isovalues_));
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
