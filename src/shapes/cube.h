#pragma once

#include "../graphics/mesh.h"

#include <vector>

struct CubeGeometry {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

CubeGeometry makeCube();