#pragma once

#include "../graphics/mesh.h"

#include <vector>

struct SphereGeometry {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

SphereGeometry makeSphere(
    unsigned int slices,
    unsigned int stacks
);