#pragma once

#include "nodes.h"
#include "../graphics/mesh.h"

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

struct CylinderGeometry;

class RoboticArm {
public:
    RoboticArm(
        const std::string& vertexShader,
        const std::string& fragmentShader,
        const std::string& sideTexture,
        const std::string& capTexture,
        const std::string& jointTexture,
        const Material& material
    );

    void update(float dt);
    void draw(
        const glm::mat4& view,
        const glm::mat4& projection
    );

    void reset();

    // Joint angles, in radians
    float baseAngle = 0.0f;
    float shoulderAngle = 0.0f;
    float elbowAngle = 0.0f;
    float wristAngle = 0.0f;
    float gripperAngle = 0.0f;
    std::string vertexShaderPath;
    std::string fragmentShaderPath;

private:
    void buildArm();
    void updateTransforms();

    // The root owns the entire tree
    std::unique_ptr<ArmNode> root;

    // Non-owning pointers to important joints
    ArmNode* shoulderNode = nullptr;
    ArmNode* elbowNode = nullptr;
    ArmNode* wristNode = nullptr;
    ArmNode* leftClawNode = nullptr;
    ArmNode* rightClawNode = nullptr;

    // Link dimensions
    float baseHeight = 0.70f;
    float baseRadius = 0.50f;

    float shoulderRadius = 0.24f;

    float upperArmLength = 2.80f;
    float upperArmRadius = 0.22f;

    float elbowRadius = 0.20f;

    float forearmLength = 2.25f;
    float forearmRadius = 0.17f;

    float wristRadius = 0.17f;

    float handLength = 0.55f;
    float handWidth = 0.50f;
    float handDepth = 0.30f;

    float clawLength = 0.70f;
    float clawWidth = 0.13f;
    float clawDepth = 0.18f;

    float clawSpacing = 0.19f;

    std::string sideTexturePath;
    std::string capTexturePath;
    std::string jointTexturePath;

    Material material;

    std::vector<std::unique_ptr<Mesh>> ownedMeshes;

    Mesh& createMesh(
        const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices,
        const std::string& texturePath
    );

    void attachCylinder(
        ArmNode& node,
        const CylinderGeometry& geometry
    );
};