#include "nodes.h"

ArmNode& ArmNode::addChild(std::unique_ptr<ArmNode> child) {
    ArmNode& reference = *child;
    children.push_back(std::move(child));
    return reference;
}

void ArmNode::draw(const glm::mat4& parentTransform, const glm::mat4& view, const glm::mat4& projection) const
{
    // Applies all parent transformations to the node
    glm::mat4 worldTransform =
        parentTransform * localTransform;

    // Calculate the transformation for this node's own geometry
    glm::mat4 modelTransform =
        worldTransform * geometryTransform;

    // Draw every mesh belonging to this node
    for (Mesh* mesh : meshes) {
        if (mesh != nullptr) {
            mesh->draw(view, projection, modelTransform);
        }
    }

    // Recursively draw children
    for (const auto& child : children) {
        child->draw(worldTransform, view, projection);
    }
}