#ifndef ATMOSPHERECOMPONENT_H
#define ATMOSPHERECOMPONENT_H

#include "../impl/AbstractComponent.h"
#include "../../../util/Serializable.h"
#include <glm/vec3.hpp>

namespace Metal {
    struct AtmosphereComponent final : AbstractComponent, Serializable {
        glm::vec3 albedo = glm::vec3(1.0f);
        glm::vec3 volumeScale = glm::vec3(100.f);
        float density = 1;
        float g = 0;
        float scatteringAlbedo = 5;
        int samples = 64;
        int volumeShadowSteps = 8;

        float sunDistance = 10000;
        float sunRadius = 3000;
        float sunLightIntensity = 2;
        float elapsedTime = .5f;
        bool atmosphereEnabled = false;
        bool volumeEnabled = false;

        glm::vec3 dawnColor{1, .39f, .19f};
        glm::vec3 nightColor{.1f, .1f, .1f};
        glm::vec3 middayColor{.9f, .9f, .9f};


        glm::vec3 sunColor{};
        glm::vec3 sunPosition{};

        void registerFields() override;

        ComponentType getType() override;

        void onUpdate(InspectableMember *member) override;

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json& j) override;
    };
} // Metal

#endif //ATMOSPHERECOMPONENT_H
