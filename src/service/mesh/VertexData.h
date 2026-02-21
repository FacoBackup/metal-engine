#ifndef VERTEXDATA_H
#define VERTEXDATA_H
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vulkan/vulkan.h>
#include "../../util/Serializable.h"

namespace Metal {
    struct VertexData final : Serializable {
        glm::vec3 vertex;
        glm::vec3 normal;
        glm::vec2 uv;

        static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(VertexData, vertex);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(VertexData, normal);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(VertexData, uv);

            return attributeDescriptions;
        }

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["v"] = {vertex.x, vertex.y, vertex.z};
            j["n"] = {normal.x, normal.y, normal.z};
            j["u"] = {uv.x, uv.y};
            return j;
        }

        void fromJson(const nlohmann::json& j) override {
            vertex = {j.at("v")[0], j.at("v")[1], j.at("v")[2]};
            normal = {j.at("n")[0], j.at("n")[1], j.at("n")[2]};
            uv = {j.at("u")[0], j.at("u")[1]};
        }
    };
}

#endif //VERTEXDATA_H
