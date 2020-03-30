#pragma once

#include <array>
#include <vector>

namespace table {
extern std::array<unsigned short, 256> const kEdgeTable;
extern std::array<std::array<short, 16>, 256> const kTriTable;
}

namespace cube {
extern std::vector<float> const cube36;
}
