#include <glad/gl.h> // Must be first!

#include <SFML/Window.hpp>
#include <SFML/System/Clock.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <exception>
#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

#include "arm/robotic_arm.h"


int main(int argc, char* argv[])
{
    // robotic_arm <side-cylinder-texture> <caps-texture> <joint-texture>

    if (argc != 3 && argc != 4) {
        std::cerr
            << "Usage: robotic_arm <side-cylinder-texture> <caps-texture> <joint-texture>\n";

        return 2;
    }

    try {
        sf::ContextSettings settings;
        settings.majorVersion = 4;
        settings.minorVersion = 0;
        settings.attributeFlags = sf::ContextSettings::Core;
        settings.depthBits = 24;

        sf::Window window(
            sf::VideoMode({800, 800}),
            "Procedural Robotic Arm",
            sf::Style::Default,
            sf::State::Windowed,
            settings
        );

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
        glDepthFunc(GL_LESS);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        const std::filesystem::path executableDirectory =
            std::filesystem::absolute(argv[0]).parent_path();

        const std::filesystem::path shaderDirectory =
            executableDirectory / "shaders";

        const std::string vertexShader =
            (shaderDirectory / "mesh.vert").string();

        const std::string fragmentShader =
            (shaderDirectory / "mesh.frag").string();

        const std::string sideTexture = argv[1];
        const std::string capTexture = argv[2];

        // If no separate joint texture is supplied, reuse the cap texture.
        const std::string jointTexture =
            argc == 4 ? argv[3] : capTexture;

        Material material;
        material.lightingEnabled = true;

        material.ambient = glm::vec3(0.2f);
        material.diffuse = glm::vec3(0.8f);
        material.specular = glm::vec3(0.5f);
        material.shininess = 32.0f;

        RoboticArm robot(
            vertexShader,
            fragmentShader,
            sideTexture,
            capTexture,
            jointTexture,
            material
        );

        robot.gripperAngle = glm::radians(0.0f); //TMP TO TEST

        constexpr float fieldOfView = glm::radians(55.0f);

        // Aim near the middle of the arm assembly.
        const glm::vec3 cameraTarget(0.0f, 3.2f, 0.0f);

        // Viewing direction.
        const glm::vec3 cameraDirection = glm::normalize(
            glm::vec3(7.0f, 4.0f, 9.0f)
        );

        // Fixed camera distance.
        constexpr float cameraDistance = 14.5f;

        const glm::vec3 cameraPosition =
            cameraTarget + cameraDirection * cameraDistance;

        const glm::vec3 worldUp(0.0f, 1.0f, 0.0f);

        const glm::mat4 view = glm::lookAt(
            cameraPosition,
            cameraTarget,
            worldUp
        );


        sf::Clock frameClock;

        const float rotationSpeed = glm::radians(30.0f);

        while (window.isOpen()) {
            while (const std::optional event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                } else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                    glViewport(
                        0,
                        0,
                        static_cast<GLsizei>(resized->size.x),
                        static_cast<GLsizei>(resized->size.y)
                    );
                } else if (const auto* keyPressed =
                    event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->code == sf::Keyboard::Key::Escape) {
                        window.close();
                    } else if (keyPressed->code == sf::Keyboard::Key::Space) {
                        robot.reset();
                    }
                }
            }

            if (!window.isOpen()) {
                break;
            }

            const float dt = frameClock.restart().asSeconds();

            // Base rotation: A / D

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
                robot.baseAngle += rotationSpeed * dt;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
                robot.baseAngle -= rotationSpeed * dt;
            }


            // Shoulder rotation: W / S

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
                robot.shoulderAngle += rotationSpeed * dt;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
                robot.shoulderAngle -= rotationSpeed * dt;
            }


            // Elbow rotation: R / F

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
                robot.elbowAngle += rotationSpeed * dt;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F)) {
                robot.elbowAngle -= rotationSpeed * dt;
            }


            // Wrist rotation: T / G

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::T)) {
                robot.wristAngle += rotationSpeed * dt;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::G)) {
                robot.wristAngle -= rotationSpeed * dt;
            }


            // Gripper: O / C

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::O)) {
                robot.gripperAngle += rotationSpeed * dt;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C)) {
                robot.gripperAngle -= rotationSpeed * dt;
            }


            constexpr float maxJointPitch = glm::radians(75.0f);
            constexpr float maxGripperAngle = glm::radians(7.0f);
            constexpr float minGripperAngle = glm::radians(-7.0f);

            // Shoulder: keep the upper arm above horizontal.
            robot.shoulderAngle = std::clamp(
                robot.shoulderAngle,
                -maxJointPitch,
                maxJointPitch
            );

            // Elbow: constrain the forearm's absolute pitch.
            robot.elbowAngle = std::clamp(
                robot.elbowAngle,
                -maxJointPitch,
                maxJointPitch
            );

            // Wrist: constrain the palm's absolute pitch.
            robot.wristAngle = std::clamp(
                robot.wristAngle,
                -maxJointPitch,
                maxJointPitch
            );

            // Gripper: allow opening from -15 to 30 degrees.
            robot.gripperAngle = std::clamp(
            robot.gripperAngle,
            minGripperAngle,
            maxGripperAngle
        );

            robot.update(dt);

            const sf::Vector2u size = window.getSize();

            if (size.x == 0 || size.y == 0) {
                continue;
            }

            const float aspectRatio =
                static_cast<float>(size.x) /
                static_cast<float>(size.y);

            const glm::mat4 projection = glm::perspective(
                fieldOfView,
                aspectRatio,
                0.1f,
                50.0f
            );


            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            robot.draw(view, projection);
            window.display();
        }

    }
    catch (const std::exception& error) {

        std::cerr << "Application failed: " << error.what() << std::endl;
        return 1;
    }

    return 0;
}