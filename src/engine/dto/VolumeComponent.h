#ifndef VOLUMECOMPONENT_H
#define VOLUMECOMPONENT_H

#include "AbstractComponent.h"
#include "../../common/ISerialize.h"
#include <glm/vec3.hpp>

namespace Metal {
    struct VolumeComponent final : AbstractComponent, ISerialize {
        glm::vec3 albedo = glm::vec3(1.0f);
        float density = 1;
        float g = 0;
        float scatteringAlbedo = 5;
        int samples = 64;

        void registerFields() override;

        ComponentType getType() const override;

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["entityId"] = entityId;
            j["albedo"] = {albedo.x, albedo.y, albedo.z};
            j["density"] = density;
            j["g"] = g;
            j["scatteringAlbedo"] = scatteringAlbedo;
            j["samples"] = samples;
            return j;
        }

        void fromJson(const nlohmann::json& j) override {
            entityId = j.at("entityId").get<entt::entity>();
            albedo = {j.at("albedo")[0], j.at("albedo")[1], j.at("albedo")[2]};
            density = j.at("density").get<float>();
            g = j.at("g").get<float>();
            scatteringAlbedo = j.at("scatteringAlbedo").get<float>();
            samples = j.at("samples").get<int>();
        }
    };
} // Metal


#endif //VOLUMECOMPONENT_H
