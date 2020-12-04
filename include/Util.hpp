#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <regex>
#include <string>
#include <vector>

namespace util
{

namespace vec
{
template<std::size_t S>
std::array<float, S>
Normalize(std::array<float, S> vec);

template<std::size_t S>
float
Magnitude(std::array<float, S> const& vec);

template<std::size_t S>
std::array<float, S>
Scale(std::array<float, S> const& vec, float scale);
}

namespace str
{
std::vector<std::string>
Split(std::string const& target, std::string const& pattern);

bool
StartsWith(std::string const& target, std::string const& prefix);

std::string
TrimCR(std::string string);
}

namespace endian
{
template<typename T>
T
BigToLittle(T const& value);
}
}

template<std::size_t S>
std::array<float, S>
util::vec::Normalize(std::array<float, S> vec)
{
  std::array<float, S> unitVec = vec;

  float const mag = util::vec::Magnitude(vec);
  for (float& n : unitVec) {
    n = n / mag;
  }

  return unitVec;
}

template<std::size_t S>
float
util::vec::Magnitude(std::array<float, S> const& vec)
{
  float sqSum = 0;

  for (auto n : vec) {
    sqSum += n * n;
  }

  return std::sqrt(sqSum);
}

template<std::size_t S>
std::array<float, S>
util::vec::Scale(std::array<float, S> const& vec, float scale)
{
  auto result = vec;

  for (float& n : result) {
    n = n * scale;
  }

  return result;
}

template<typename T>
inline T
util::endian::BigToLittle(T const& value)
{
  constexpr std::size_t count = sizeof(T);

  T result;

  char const* const big = reinterpret_cast<char const*>(&value);
  char* const little = reinterpret_cast<char*>(&result);
  /**
   * Example: sizeof(float) is 4
   *
   * little[0] = big[3]
   * little[1] = big[2]
   * little[2] = big[1]
   * little[3] = big[0]
   */
  for (std::size_t i = 0; i < count; ++i) {
    little[i] = big[count - (i + 1)];
  }

  return result;
}
