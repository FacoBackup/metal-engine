#ifndef ENGINEREPO_H
#define ENGINEREPO_H
#include "../../common/inspection/Inspectable.h"
#include <glm/vec3.hpp>
#include "../../util/serialization-definitions.h"

namespace Metal {
    struct EngineRepository final : Inspectable {
        bool giEnabled = false;
        int giTileSubdivision = 20;
        int giBounces = 1;
        int giSamplesPerPixel = 1;
        float giStrength = 1;
        int numberOfTiles = 10;
        float elapsedTime = .5f;
        bool atmosphereEnabled = false;
        bool incrementTime = false;
        float elapsedTimeSpeed = 1;
        float sunDistance = 100000;
        float sunLightIntensity = 2;
        glm::vec3 dawnColor{1, .39f, .19f};
        glm::vec3 nightColor{.1f, .1f, .1f};
        glm::vec3 middayColor{.9f, .9f, .9f};
        bool screenSpaceShadows = false;
        std::vector<std::string> svoFilePaths{};

        void registerFields() override;

        const char *getIcon() override;

        const char *getTitle() override;

        void onUpdate(InspectableMember *member, ApplicationContext &context) override;

        SAVE_TEMPLATE(
            giBounces,
            atmosphereEnabled,
            elapsedTime,
            incrementTime,
            elapsedTimeSpeed,
            sunDistance,
            sunLightIntensity,
            dawnColor.x, dawnColor.y, dawnColor.z,
            nightColor.x, nightColor.y, nightColor.z,
            middayColor.x, middayColor.y, middayColor.z,
            screenSpaceShadows,
            svoFilePaths,
            giEnabled,
            giTileSubdivision,
            giSamplesPerPixel,
            giStrength
        )
    };
} // Metal

#endif //ENGINEREPO_H
