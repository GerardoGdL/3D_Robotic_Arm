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
    ArmNode& palmNode = wristNode->addChild(
        std::make_unique<ArmNode>()
    );

    // LEFT STEM
    leftStemNode = &palmNode.addChild(
        std::make_unique<ArmNode>()
    );

    // LEFT FINGER JOINT
    leftFingerJointNode = &leftStemNode->addChild(
        std::make_unique<ArmNode>()
    );

    // LEFT FINGER
    leftFingerNode = &leftFingerJointNode->addChild(
        std::make_unique<ArmNode>()
    );

    // RIGHT STEM
    rightStemNode = &palmNode.addChild(
        std::make_unique<ArmNode>()
    );

    // RIGHT FINGER JOINT
    rightFingerJointNode = &rightStemNode->addChild(
        std::make_unique<ArmNode>()
    );

    // RIGHT FINGER
    rightFingerNode = &rightFingerJointNode->addChild(
        std::make_unique<ArmNode>()
    );

    // Base
    CylinderGeometry base = makeCylinder(
        16,
        baseHeight,
        baseRadius
    );

    attachCylinder(*root, base, "", capTexturePath, "");

    // Upper arm
    CylinderGeometry upperArm = makeCylinder(
        32,
        upperArmLength,
        upperArmRadius
    );

    attachCylinder(upperArmNode, upperArm, sideTexturePath, "", "");

    // Forearm
    CylinderGeometry forearm = makeCylinder(
        32,
        forearmLength,
        forearmRadius
    );

    attachCylinder(forearmNode, forearm, sideTexturePath, "", "");

    SphereGeometry sphere = makeSphere(32, 16);

    // Creates one sphere Mesh and reuses it for all joints.
    Mesh& jointMesh = createMesh(
        sphere.vertices,
        sphere.indices,
        ""
    );

    shoulderNode->meshes.push_back(&jointMesh);
    elbowNode->meshes.push_back(&jointMesh);
    wristNode->meshes.push_back(&jointMesh);

    CubeGeometry cube = makeCube();

    // Reuse one cube mesh for the hand and both claws.
    Mesh& gripperMesh = createMesh(
        cube.vertices,
        cube.indices,
        ""
    );

    // Palm
    palmNode.meshes.push_back(&gripperMesh);

    // Rotating fingers
    leftFingerNode->meshes.push_back(&gripperMesh);
    rightFingerNode->meshes.push_back(&gripperMesh);

    // --------------------------------
    // Geometry offsets
    // --------------------------------

    // --------------------------------
    // FIXED STEM CYLINDERS
    // --------------------------------

    CylinderGeometry stem = makeCylinder(
        16,
        stemLength,
        stemRadius
    );

    // Create the cylinder meshes
    Mesh& stemSideMesh = createMesh(
        stem.sideVertices,
        stem.sideIndices,
        ""
    );

    Mesh& stemTopMesh = createMesh(
        stem.topVertices,
        stem.topIndices,
        ""
    );

    Mesh& stemBottomMesh = createMesh(
        stem.bottomVertices,
        stem.bottomIndices,
        ""
    );

    // Attach the same cylinder meshes to both fixed stems.
    leftStemNode->meshes.push_back(&stemSideMesh);
    leftStemNode->meshes.push_back(&stemTopMesh);
    leftStemNode->meshes.push_back(&stemBottomMesh);

    rightStemNode->meshes.push_back(&stemSideMesh);
    rightStemNode->meshes.push_back(&stemTopMesh);
    rightStemNode->meshes.push_back(&stemBottomMesh);

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

    // --------------------------------
    // HAND / PALM
    // --------------------------------

    palmNode.geometryTransform =
        glm::translate(
            glm::mat4(1.0f),
            glm::vec3(0.0f, palmLength / 2.0f, 0.0f)
        ) *
        glm::scale(
            glm::mat4(1.0f),
            glm::vec3(
                palmWidth / 2.0f,
                palmLength / 2.0f,
                palmHeight / 2.0f
            )
        );

    // --------------------------------
    // FIXED STEMS
    // --------------------------------

    glm::mat4 stemGeometryTransform =
    glm::translate(
        glm::mat4(1.0f),
        glm::vec3(0.0f, stemLength / 2.0f, 0.0f)
    );

    leftStemNode->geometryTransform = stemGeometryTransform;
    rightStemNode->geometryTransform = stemGeometryTransform;

    // --------------------------------
    // FINGER JOINTS
    // --------------------------------

    leftFingerJointNode->geometryTransform =
        glm::scale(
            glm::mat4(1.0f),
            glm::vec3(fingerJointRadius)
        );

    rightFingerJointNode->geometryTransform =
        glm::scale(
            glm::mat4(1.0f),
            glm::vec3(fingerJointRadius)
        );

    // --------------------------------
    // FINGERS
    // --------------------------------

    leftFingerNode->geometryTransform =
        glm::translate(
            glm::mat4(1.0f),
            glm::vec3(0.0f, fingerLength / 2.0f, 0.0f)
        ) *
        glm::scale(
            glm::mat4(1.0f),
            glm::vec3(
                fingerWidth / 2.0f,
                fingerLength / 2.0f,
                fingerThickness / 2.0f
            )
        );

    rightFingerNode->geometryTransform =
        glm::translate(
            glm::mat4(1.0f),
            glm::vec3(0.0f, fingerLength / 2.0f, 0.0f)
        ) *
        glm::scale(
            glm::mat4(1.0f),
            glm::vec3(
                fingerWidth / 2.0f,
                fingerLength / 2.0f,
                fingerThickness / 2.0f
            )
        );
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

    // --------------------------------
    // FIXED STEM POSITIONS
    // --------------------------------

    float stemOffset = palmWidth / 2.0f - stemRadius;

    leftStemNode->localTransform =
        glm::translate(
            glm::mat4(1.0f),
            glm::vec3(0.0f, palmLength, -stemOffset)
        );

    rightStemNode->localTransform =
        glm::translate(
            glm::mat4(1.0f),
            glm::vec3(0.0f, palmLength, stemOffset)
        );

    // --------------------------------
    // FINGER JOINT POSITIONS + ROTATIONS
    // --------------------------------

    leftFingerJointNode->localTransform =
        glm::translate(
            glm::mat4(1.0f),
            glm::vec3(0.0f, stemLength, 0.0f)
        ) *
        glm::rotate(
            glm::mat4(1.0f),
            -gripperAngle,
            glm::vec3(1.0f, 0.0f, 0.0f)
        );

    rightFingerJointNode->localTransform =
        glm::translate(
            glm::mat4(1.0f),
            glm::vec3(0.0f, stemLength, 0.0f)
        ) *
        glm::rotate(
            glm::mat4(1.0f),
            gripperAngle,
            glm::vec3(1.0f, 0.0f, 0.0f)
        );
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
    const CylinderGeometry& geometry,
    const std::string& sideTexture,
    const std::string& topTexture,
    const std::string& bottomTexture
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