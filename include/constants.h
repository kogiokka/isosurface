#pragma once

#include <array>
#include <vector>

namespace table
{
extern std::array<short, 256> const kEdgeTable;
extern std::array<std::array<short, 16>, 256> const kTriTable;
}
