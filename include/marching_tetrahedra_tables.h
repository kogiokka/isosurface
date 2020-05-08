#pragma once

#include <array>

namespace tetr
{
extern std::array<std::array<short, 4>, 6> const kTetrahedraVertices;
extern std::array<short, 6> const kTetrahedraVertexBytes;
extern std::array<short, 16> const kTetrahedraEdges;
extern std::array<std::array<short, 7>, 16> const kTetrahedraTriangles;
}
