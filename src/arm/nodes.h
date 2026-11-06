#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <vector>

#include "../graphics/mesh.h"

class ArmNode {
    public:
        // Transformation relative to the parent node
        glm::mat4 localTransform{1.0f};

        // Offset of this node's geometry from its origin
        glm::mat4 geometryTransform{1.0f};

        // A node can have multiple meshes but must be treated as one for the transformations
        std::vector<Mesh*> meshes;

        // Children in the kinematic tree
        std::vector<std::unique_ptr<ArmNode>> children;

        ArmNode& addChild(std::unique_ptr<ArmNode> child);

        void draw(
            const glm::mat4& parentTransform,
            const glm::mat4& view,
            const glm::mat4& projection
        ) const;
};