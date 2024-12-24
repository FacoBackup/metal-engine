#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H
#include <glm/vec3.hpp>
#include "../../../util/serialization-definitions.h"

namespace Metal {
    struct BoundingBox final {
        glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());
        glm::vec3 center = glm::vec3(0.0f);

        bool intersects(const BoundingBox& bb) const {
            return (min.x < bb.max.x && max.x > bb.min.x &&
                    min.y < bb.max.y && max.y > bb.min.y &&
                    min.z < bb.max.z && max.z > bb.min.z);
        }

        bool intersects(const glm::vec3& point) const {
            return (min.x <= point.x && max.x >= point.x &&
                    min.y <= point.y && max.y >= point.y &&
                    min.z <= point.z && max.z >= point.z);
        }

        SERIALIZE_TEMPLATE(
          min.x, min.y, min.z,
          max.x, max.y, max.z,
          center.x, center.y, center.z
        )
    };
} // Metal

#endif //BOUNDINGBOX_H
