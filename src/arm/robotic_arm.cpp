#include "robotic_arm.h"

#include "../shapes/cylinder.h"
#include "../shapes/sphere.h"
#include "../shapes/cube.h"

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

RoboticArm::RoboticArm(
    const std::string& vertexShader,
    const std::string& fragmentShader,
    const std::string& sideTexture,
    const std::string& capTexture,
    const std::string& jointTexture,
    const Material& material
)
    : vertexShaderPath(vertexShader),
      fragmentShaderPath(fragmentShader),
      sideTexturePath(sideTexture),
      capTexturePath(capTexture),
      jointTexturePath(jointTexture),
      material(material)
{
    buildArm();
    updateTransforms();
}

void RoboticArm::buildArm() {

    // ROOT: Base
    root = std::make_unique<ArmNode>();

    // SHOULDER
    shoulderNode = &root->addChild(
        std::make_unique<ArmNode>()
    );

    // UPPER ARM
    ArmNode& upperArmNode = shoulderNode->addChild(
        std::make_unique<ArmNode>()
    );

    // ELBOW
    elbowNode = &upperArmNode.addChild(
        std::make_unique<ArmNode>()
    );

    // FOREARM
    ArmNode& forearmNode = elbowNode->addChild(
        std::make_unique<ArmNode>()
    );

    // WRIST
    wristNode = &forearmNode.addChild(
        std::make_unique<ArmNode>()
    );

    // HAND
    ArmNode& handNode = wristNode->addChild(
        std::make_unique<ArmNode>()
    );


    CylinderGeometry base = makeCylinder(
        32,
        baseHeight,
        baseRadius
    );

    attachCylinder(*root, base);

    CylinderGeometry upperArm = makeCylinder(
        32,
        upperArmLength,
        upperArmRadius
    );

    attachCylinder(upperArmNode, upperArm);

    CylinderGeometry forearm = makeCylinder(
        32,
        forearmLength,
        forearmRadius
    );

    attachCylinder(forearmNode, forearm);

    SphereGeometry sphere = makeSphere(32, 16);

    // Creates one sphere Mesh and reuses it for all joints.
    Mesh& jointMesh = createMesh(
        sphere.vertices,
        sphere.indices,
        jointTexturePath
    );

    shoulderNode->meshes.push_back(&jointMesh);
    elbowNode->meshes.push_back(&jointMesh);
    wristNode->meshes.push_back(&jointMesh);

    // --------------------------------
    // Geometry offsets
    // --------------------------------

    // Base cylinder is centered at its origin.
    root->geometryTransform = glm::mat4(1.0f);

    // Shoulder sphere is centered at its joint.
    shoulderNode->geometryTransform = glm::scale(
        glm::mat4(1.0f),
        glm::vec3(shoulderRadius)
    );

    // Upper arm cylinder is centered halfway along its length.
    upperArmNode.geometryTransform = glm::translate(
        glm::mat4(1.0f),
        glm::vec3(0.0f, upperArmLength / 2.0f, 0.0f)
    );

    // Elbow sphere is centered at its joint.
    elbowNode->geometryTransform = glm::scale(
        glm::mat4(1.0f),
        glm::vec3(elbowRadius)
    );

    // Forearm cylinder is centered halfway along its length.
    forearmNode.geometryTransform = glm::translate(
        glm::mat4(1.0f),
        glm::vec3(0.0f, forearmLength / 2.0f, 0.0f)
    );

    // Wrist sphere is centered at its joint.
    wristNode->geometryTransform = glm::scale(
        glm::mat4(1.0f),
        glm::vec3(wristRadius)
    );

    handNode.geometryTransform = glm::mat4(1.0f);
}

void RoboticArm::updateTransforms() {

    // --------------------------------
    // BASE
    // --------------------------------

    root->localTransform = glm::rotate(
        glm::mat4(1.0f),
        baseAngle,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );


    // --------------------------------
    // SHOULDER
    // --------------------------------

    glm::mat4 shoulderTranslation = glm::translate(
        glm::mat4(1.0f),
        glm::vec3(0.0f, baseHeight / 2.0f, 0.0f)
    );

    glm::mat4 shoulderRotation = glm::rotate(
        glm::mat4(1.0f),
        shoulderAngle,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    shoulderNode->localTransform =
        shoulderTranslation * shoulderRotation;


    // --------------------------------
    // ELBOW
    // --------------------------------

    glm::mat4 elbowTranslation = glm::translate(
        glm::mat4(1.0f),
        glm::vec3(0.0f, upperArmLength, 0.0f)
    );

    glm::mat4 elbowRotation = glm::rotate(
        glm::mat4(1.0f),
        elbowAngle,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    elbowNode->localTransform =
        elbowTranslation * elbowRotation;


    // --------------------------------
    // WRIST
    // --------------------------------

    glm::mat4 wristTranslation = glm::translate(
        glm::mat4(1.0f),
        glm::vec3(0.0f, forearmLength, 0.0f)
    );

    glm::mat4 wristRotation = glm::rotate(
        glm::mat4(1.0f),
        wristAngle,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    wristNode->localTransform =
        wristTranslation * wristRotation;
}

void RoboticArm::draw(
    const glm::mat4& view,
    const glm::mat4& projection
) {
    if (root) {
        root->draw(
            glm::mat4(1.0f),
            view,
            projection
        );
    }
}

void RoboticArm::update(float dt) {
    // Update joint angles here when you implement input.
    // dt is the elapsed frame time.

    updateTransforms();
}

void RoboticArm::reset() {
    baseAngle = 0.0f;
    shoulderAngle = 0.0f;
    elbowAngle = 0.0f;
    wristAngle = 0.0f;
    gripperAngle = 0.0f;

    updateTransforms();
}

Mesh& RoboticArm::createMesh(
    const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices,
    const std::string& texturePath
) {
    auto mesh = std::make_unique<Mesh>(
        vertices,
        indices,
        vertexShaderPath,
        fragmentShaderPath,
        texturePath,
        material
    );

    Mesh& reference = *mesh;

    ownedMeshes.push_back(std::move(mesh));

    return reference;
}

void RoboticArm::attachCylinder(
    ArmNode& node,
    const CylinderGeometry& geometry
) {
    // Curved side
    node.meshes.push_back(&createMesh(
        geometry.sideVertices,
        geometry.sideIndices,
        sideTexturePath
    ));

    // Top cap
    node.meshes.push_back(&createMesh(
        geometry.topVertices,
        geometry.topIndices,
        capTexturePath
    ));

    // Bottom cap
    node.meshes.push_back(&createMesh(
        geometry.bottomVertices,
        geometry.bottomIndices,
        capTexturePath
    ));
}