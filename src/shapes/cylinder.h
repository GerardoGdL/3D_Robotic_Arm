#pragma once

#include "../graphics/mesh.h"

#include <vector>

struct CylinderGeometry {
    std::vector<Vertex> sideVertices;
    std::vector<unsigned int> sideIndices;

    std::vector<Vertex> topVertices;
    std::vector<unsigned int> topIndices;

    std::vector<Vertex> bottomVertices;
    std::vector<unsigned int> bottomIndices;

    float height;
    float radius;
};

CylinderGeometry makeCylinder(
    unsigned int slices,
    float height,
    float radius
);