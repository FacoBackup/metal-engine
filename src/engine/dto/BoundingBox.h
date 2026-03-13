#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H
#include <glm/vec3.hpp>
#include "../../../common/Serializable.h"

namespace Metal {
    struct BoundingBox final : Serializable {
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

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["min"] = {min.x, min.y, min.z};
            j["max"] = {max.x, max.y, max.z};
            j["center"] = {center.x, center.y, center.z};
            return j;
        }

        void fromJson(const nlohmann::json& j) override {
            min = {j.at("min")[0], j.at("min")[1], j.at("min")[2]};
            max = {j.at("max")[0], j.at("max")[1], j.at("max")[2]};
            center = {j.at("center")[0], j.at("center")[1], j.at("center")[2]};
        }
    };
} // Metal

#endif //BOUNDINGBOX_H
