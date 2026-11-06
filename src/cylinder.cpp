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
    // By separating the indeces of each face, we can assign them to different meshes with different materials and shaders.
    std::vector<Vertex> sideVertices;
    std::vector<unsigned int> sideIndices;

    std::vector<Vertex> topVertices;
    std::vector<unsigned int> topIndices;

    std::vector<Vertex> bottomVertices;
    std::vector<unsigned int> bottomIndices;
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

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: cylinder <cap-texture> <side-texture>\n";
        return 2;
    }

    try {
        sf::ContextSettings settings;
        settings.majorVersion = 4;
        settings.minorVersion = 0;
        settings.attributeFlags = sf::ContextSettings::Core;
        settings.depthBits = 24;

        sf::Window window(sf::VideoMode({800, 800}),
                          "Textured cylinder",
                          sf::Style::Default,
                          sf::State::Windowed,
                          settings);

        const sf::ContextSettings actualSettings = window.getSettings();
        if (actualSettings.majorVersion < 4) {
            throw std::runtime_error("OpenGL 4.0 Core is required, but the created context is " +
                                     std::to_string(actualSettings.majorVersion) + "." +
                                     std::to_string(actualSettings.minorVersion));
        }

        if (!gladLoadGL(sf::Context::getFunction)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            return 1;
        }

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        CylinderGeometry cylinder = makeCylinder(16, 0.7, 0.5f);
        const std::filesystem::path shaderDirectory =
            std::filesystem::absolute(argv[0]).parent_path() / "shaders";
        Material material;
        material.lightingEnabled = false;
        Mesh sideMesh(cylinder.sideVertices,
                      cylinder.sideIndices,
                      (shaderDirectory / "mesh.vert").string(),
                      (shaderDirectory / "mesh.frag").string(),
                      argv[2],
                      material);

        Mesh topMesh(cylinder.topVertices,
                     cylinder.topIndices,
                     (shaderDirectory / "mesh.vert").string(),
                     (shaderDirectory / "mesh.frag").string(),
                     argv[1],
                     material);

        Mesh bottomMesh(cylinder.bottomVertices,
                        cylinder.bottomIndices,
                        (shaderDirectory / "mesh.vert").string(),
                        (shaderDirectory / "mesh.frag").string(),
                        argv[1],
                        material);

        constexpr float fieldOfView = glm::radians(45.0f);
        const glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f);
        const float cylinderRadius = 0.7f;
        const glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
        sf::Clock frameClock;
        const float rotationSpeed = glm::radians(30.0f);

        while (window.isOpen()) {
            while (const std::optional event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                } else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                    glViewport(0,
                               0,
                               static_cast<GLsizei>(resized->size.x),
                               static_cast<GLsizei>(resized->size.y));
                } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->code == sf::Keyboard::Key::Left) {
                        rotate(sideMesh, glm::radians(5.0f));
                        rotate(topMesh, glm::radians(5.0f));
                        rotate(bottomMesh, glm::radians(5.0f));
                    } else if (keyPressed->code == sf::Keyboard::Key::Right) {
                        rotate(sideMesh, glm::radians(-5.0f));
                        rotate(topMesh, glm::radians(-5.0f));
                        rotate(bottomMesh, glm::radians(-5.0f));
                    } else if (keyPressed->code == sf::Keyboard::Key::Escape) {
                        window.close();
                    }
                }
            }

            if (!window.isOpen()) {
                break;
            }

            const float dt = frameClock.restart().asSeconds();
            rotate(sideMesh, rotationSpeed * dt);
            rotate(topMesh, rotationSpeed * dt);
            rotate(bottomMesh, rotationSpeed * dt);

            const sf::Vector2u size = window.getSize();
            glViewport(0, 0, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y));
            const float aspectRatio = size.x == 0 || size.y == 0
                ? 1.0f
                : static_cast<float>(size.x) / static_cast<float>(size.y);
            const float limitingAspect = std::min(aspectRatio, 1.0f);
            const float limitingHalfFov = std::atan(
                std::tan(fieldOfView * 0.5f) * limitingAspect);
            const float cameraDistance = cylinderRadius / std::sin(limitingHalfFov) * 1.1f;
            const glm::mat4 view = glm::lookAt(cameraTarget + glm::vec3(0.0f,
                                                                        0.0f,
                                                                        cameraDistance),
                                               cameraTarget,
                                               worldUp);
            const glm::mat4 projection = glm::perspective(fieldOfView,
                                                           aspectRatio,
                                                           0.1f,
                                                           cameraDistance + cylinderRadius * 2.0f);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            sideMesh.draw(view, projection);
            topMesh.draw(view, projection);
            bottomMesh.draw(view, projection);
            window.display();
        }
    } catch (const std::exception& error) {
        std::cerr << "Application failed: " << error.what() << std::endl;
        return 1;
    }

    return 0;
}