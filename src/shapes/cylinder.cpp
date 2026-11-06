#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <vector>

#include "../graphics/mesh.h"
#include "cylinder.h"

CylinderGeometry makeCylinder(unsigned int slices, float height, float radius)
{
    constexpr float pi = 3.14159265358979323846f;
    CylinderGeometry cylinder;

    cylinder.height = height;
    cylinder.radius = radius;
    float halfHeight = cylinder.height / 2.0f; // The cylinder will be generated centered at the origin, so must translate the top and bottom caps by halfHeight
    
    // Making the side vertices like the sphere, but with a constant y value for the top and bottom
    for (unsigned int slice = 0; slice <= slices; ++slice) {
        const float i = static_cast<float>(slice) / static_cast<float>(slices);
        const float theta = 2.0f * pi * i;
        
        float x = radius * std::cos(theta);
        float z = radius * std::sin(theta);

        // The normal will point outwards from the center
        glm::vec3 normal(
            std::cos(theta),
            0.0f,
            std::sin(theta)
        );

        float u = static_cast<float>(slice) / slices;

        // Bottom vertex
        Vertex bottom;
        bottom.position = glm::vec3(x, -halfHeight, z);
        bottom.normal = normal;
        bottom.texCoord = glm::vec2(u, 0.0f);

        cylinder.sideVertices.push_back(bottom);

        // Top vertex
        Vertex top;
        top.position = glm::vec3(x, halfHeight, z);
        top.normal = normal;
        top.texCoord = glm::vec2(u, 1.0f);
        top.color = glm::vec4(1.0f);

        cylinder.sideVertices.push_back(top);
    }

    // Now we need to create the indices for the side triangles
    for (unsigned int i = 0; i < slices; ++i)
    {
        unsigned int bottom = i * 2;
        unsigned int top = bottom + 1;

        unsigned int nextBottom = bottom + 2;
        unsigned int nextTop = bottom + 3;

        cylinder.sideIndices.push_back(bottom);
        cylinder.sideIndices.push_back(top);
        cylinder.sideIndices.push_back(nextBottom);

        cylinder.sideIndices.push_back(nextBottom);
        cylinder.sideIndices.push_back(top);
        cylinder.sideIndices.push_back(nextTop);
    }

    // Now we need to create the top and bottom caps
    // Bottom cap

    Vertex bottomCenter;
    bottomCenter.position = glm::vec3(0.0f, -halfHeight, 0.0f);
    bottomCenter.normal = glm::vec3(0.0f, -1.0f, 0.0f);
    bottomCenter.texCoord = glm::vec2(0.5f, 0.5f);

    cylinder.bottomVertices.push_back(bottomCenter);

    for (int i = 0; i <= slices; i++) {
        float theta = 2.0f * pi * i / slices;

        float x = radius * cos(theta);
        float z = radius * sin(theta);

        Vertex vertex;
        vertex.position = glm::vec3(x, -halfHeight, z);
        vertex.normal = glm::vec3(0.0f, -1.0f, 0.0f);

        vertex.texCoord = glm::vec2(
            0.5f + 0.5f * cos(theta),
            0.5f + 0.5f * sin(theta)
        );

        cylinder.bottomVertices.push_back(vertex);
    }

    for (int i = 0; i < slices; i++) {
        cylinder.bottomIndices.push_back(0);
        cylinder.bottomIndices.push_back(i + 2);
        cylinder.bottomIndices.push_back(i + 1);
    }

    // Top cap
    Vertex topCenter;
    topCenter.position = glm::vec3(0.0f, halfHeight, 0.0f);
    topCenter.normal = glm::vec3(0.0f, 1.0f, 0.0f);
    topCenter.texCoord = glm::vec2(0.5f, 0.5f);

    cylinder.topVertices.push_back(topCenter);

    for (unsigned int i = 0; i <= slices; ++i)
    {
        float theta = 2.0f * pi * i / slices;

        float x = radius * std::cos(theta);
        float z = radius * std::sin(theta);

        Vertex vertex;
        vertex.position = glm::vec3(x, halfHeight, z);
        vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        vertex.texCoord = glm::vec2(
            0.5f + 0.5f * cos(theta),
            0.5f + 0.5f * sin(theta)
        );

        cylinder.topVertices.push_back(vertex);
    }

    for (int i = 0; i < slices; i++) {
        cylinder.topIndices.push_back(0);
        cylinder.topIndices.push_back(i + 1);
        cylinder.topIndices.push_back(i + 2);
    }

    return cylinder;
}

void rotate(Mesh& mesh, float radians)
{
    mesh.applyTransform(glm::rotate(glm::mat4(1.0f),
                                    radians,
                                    glm::vec3(0.0f, 1.0f, 0.0f)));
};