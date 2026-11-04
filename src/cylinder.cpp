#include <glad/gl.h> // Must be first!
#include <SFML/Window.hpp>
#include <SFML/System/Clock.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>
#include <exception>
#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "mesh.h"

struct CylinderGeometry {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    float height;
    float radius;
};

void rotate(Mesh& mesh, float radians)
{
    mesh.applyTransform(glm::rotate(glm::mat4(1.0f),
                                    radians,
                                    glm::vec3(0.0f, 1.0f, 0.0f)));
};

CylinderGeometry makeCylinder(unsigned int slices, float height, float radius)
{
    constexpr float pi = 3.14159265358979323846f;
    CylinderGeometry cylinder;
    cylinder.vertices.reserve(static_cast<std::size_t>(4 * slices + 6));
    cylinder.indices.reserve(static_cast<std::size_t>(12 * slices));

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
        cylinder.vertices.push_back({
            glm::vec3(x, -halfHeight, z),
            normal,
            glm::vec2(u, 0.0f),
            glm::vec4(1.0f)
        });

        // Top vertex
        cylinder.vertices.push_back({
            glm::vec3(x, halfHeight, z),
            normal,
            glm::vec2(u, 1.0f),
            glm::vec4(1.0f)
        });


        // Now we need to create the indices for the side triangles
        for (unsigned int i = 0; i < slices; ++i)
        {
            unsigned int bottom = i * 2;
            unsigned int top = bottom + 1;

            unsigned int nextBottom = bottom + 2;
            unsigned int nextTop = bottom + 3;

            cylinder.indices.insert(
                cylinder.indices.end(),
                {
                    bottom, top, nextBottom,
                    nextBottom, top, nextTop
                }
            );
        }

        // Now we need to create the top and bottom caps
        // Bottom cap
        unsigned int bottomCenter =
        static_cast<unsigned int>(cylinder.vertices.size());

        cylinder.vertices.push_back({
            glm::vec3(0.0f, -halfHeight, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec2(0.5f),
            glm::vec4(1.0f)
        });

        unsigned int bottomStart =
            static_cast<unsigned int>(cylinder.vertices.size());

        for (unsigned int i = 0; i <= slices; ++i)
        {
            float theta = 2.0f * pi * i / slices;

            float x = radius * std::cos(theta);
            float z = radius * std::sin(theta);

            cylinder.vertices.push_back({
                glm::vec3(x, -halfHeight, z),
                glm::vec3(0.0f, -1.0f, 0.0f),
                glm::vec2(
                    0.5f + 0.5f * std::cos(theta),
                    0.5f + 0.5f * std::sin(theta)
                ),
                glm::vec4(1.0f)
            });
        }

        for (unsigned int i = 0; i < slices; ++i) {
            cylinder.indices.insert(
                cylinder.indices.end(),
                {
                    bottomCenter,
                    bottomStart + i,
                    bottomStart + i + 1
                }
            );
        }

        // Top cap
        unsigned int topCenter =
        static_cast<unsigned int>(cylinder.vertices.size());

        cylinder.vertices.push_back({
            glm::vec3(0.0f, halfHeight, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec2(0.5f),
            glm::vec4(1.0f)
        });

        unsigned int topStart =
            static_cast<unsigned int>(cylinder.vertices.size());

        for (unsigned int i = 0; i <= slices; ++i)
        {
            float theta = 2.0f * pi * i / slices;

            float x = radius * std::cos(theta);
            float z = radius * std::sin(theta);

            cylinder.vertices.push_back({
                glm::vec3(x, halfHeight, z),
                glm::vec3(0.0f, 1.0f, 0.0f),
                glm::vec2(
                    0.5f + 0.5f * std::cos(theta),
                    0.5f + 0.5f * std::sin(theta)
                ),
                glm::vec4(1.0f)
            });
        }

        for (unsigned int i = 0; i < slices; ++i) {
            cylinder.indices.insert(
                cylinder.indices.end(),
                {
                    topCenter,
                    topStart + i + 1,
                    topStart + i
                }
            );
        }

    return cylinder;
}