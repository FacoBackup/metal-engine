#ifndef ATMOSPHEREREPOSITORY_H
#define ATMOSPHEREREPOSITORY_H
#include "../../common/inspection/Inspectable.h"
#include <glm/vec3.hpp>

namespace Metal {
    struct AtmosphereRepository final : Inspectable {
        float elapsedTime = .5f;
        bool incrementTime = false;
        float elapsedTimeSpeed = 1;
        float sunDistance = 100000;
        float sunLightIntensity = 2;
        glm::vec3 dawnColor{1, .39f, .19f};
        glm::vec3 nightColor{.1f, .1f, .1f};
        glm::vec3 middayColor{.9f, .9f, .9f};
        bool screenSpaceShadows = false;

        void registerFields() override;

        const char *getIcon() override;

        const char *getTitle() override;
    };
} // Metal

#endif //ATMOSPHEREREPOSITORY_H