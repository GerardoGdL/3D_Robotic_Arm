#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "../graphics/mesh.h"
#include "cube.h"

Vertex makeVertex(const glm::vec3& position, const glm::vec2& texCoord)
{
    Vertex vertex;
    vertex.position = position;
    vertex.texCoord = texCoord;
    return vertex;
}

CubeGeometry makeCube()
{
    constexpr glm::vec2 bottomLeft(0.0f, 0.0f);
    constexpr glm::vec2 bottomRight(1.0f, 0.0f);
    constexpr glm::vec2 topRight(1.0f, 1.0f);
    constexpr glm::vec2 topLeft(0.0f, 1.0f);

    CubeGeometry cube;
    cube.vertices = {
        // Front
        makeVertex({-1.0f, -1.0f,  1.0f}, bottomLeft),
        makeVertex({ 1.0f, -1.0f,  1.0f}, bottomRight),
        makeVertex({ 1.0f,  1.0f,  1.0f}, topRight),
        makeVertex({-1.0f,  1.0f,  1.0f}, topLeft),
        // Back
        makeVertex({ 1.0f, -1.0f, -1.0f}, bottomLeft),
        makeVertex({-1.0f, -1.0f, -1.0f}, bottomRight),
        makeVertex({-1.0f,  1.0f, -1.0f}, topRight),
        makeVertex({ 1.0f,  1.0f, -1.0f}, topLeft),
        // Right
        makeVertex({ 1.0f, -1.0f,  1.0f}, bottomLeft),
        makeVertex({ 1.0f, -1.0f, -1.0f}, bottomRight),
        makeVertex({ 1.0f,  1.0f, -1.0f}, topRight),
        makeVertex({ 1.0f,  1.0f,  1.0f}, topLeft),
        // Left
        makeVertex({-1.0f, -1.0f, -1.0f}, bottomLeft),
        makeVertex({-1.0f, -1.0f,  1.0f}, bottomRight),
        makeVertex({-1.0f,  1.0f,  1.0f}, topRight),
        makeVertex({-1.0f,  1.0f, -1.0f}, topLeft),
        // Top
        makeVertex({-1.0f,  1.0f,  1.0f}, bottomLeft),
        makeVertex({ 1.0f,  1.0f,  1.0f}, bottomRight),
        makeVertex({ 1.0f,  1.0f, -1.0f}, topRight),
        makeVertex({-1.0f,  1.0f, -1.0f}, topLeft),
        // Bottom
        makeVertex({-1.0f, -1.0f, -1.0f}, bottomLeft),
        makeVertex({ 1.0f, -1.0f, -1.0f}, bottomRight),
        makeVertex({ 1.0f, -1.0f,  1.0f}, topRight),
        makeVertex({-1.0f, -1.0f,  1.0f}, topLeft)
    };

    cube.indices.reserve(36);
    for (unsigned int face = 0; face < 6; ++face) {
        const unsigned int first = face * 4;
        cube.indices.insert(cube.indices.end(),
                            {first, first + 1, first + 2,
                             first, first + 2, first + 3});
    }

    return cube;
}