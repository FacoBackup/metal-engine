#include "EngineRepository.h"

#include "../../common/interface/Icons.h"
#include "../../context/ApplicationContext.h"

#define LEVEL_OF_DETAIL "Level of detail"

namespace Metal {
    void EngineRepository::registerFields() {
        registerBool(voxelGIEnabled, "Global illumination", "Enabled?");
        registerFloat(voxelRaytracingBias, "Global illumination", "Ray tracing bias", 0, 5, false, .001);
        registerFloat(voxelHitBias, "Global illumination", "Ray tracing hit bias", 0, 10, false, .0001);
        registerFloat(shadowsBaseColor, "Global illumination", "Shadow base color", 0, 1, false, .001);
        registerInt(numberOfTiles, "World", "Number of tiles", 2, 100);
        registerFloat(elapsedTime, "Time", "Elapsed time");
        registerBool(incrementTime, "Time", "Increment time");
        registerFloat(elapsedTimeSpeed, "Time", "Time of day speed");
        registerFloat(sunDistance, "Sun", "Sun distance");
        registerFloat(sunLightIntensity, "Sun", "Light intensity");
        registerColor(dawnColor, "Sun", "Dawn color");
        registerColor(nightColor, "Sun", "Night color");
        registerColor(middayColor, "Sun", "Midday color");
        registerBool(screenSpaceShadows, "", "Screen space shadows");
    }

    void EngineRepository::onUpdate(InspectableMember *member, ApplicationContext &context) {
        if (member->name == LEVEL_OF_DETAIL) {
            context.worldGridRepository.hasMainTileChanged = true;
        }
    }

    const char *EngineRepository::getIcon() {
        return Icons::display_settings.c_str();
    }

    const char *EngineRepository::getTitle() {
        return "Engine settings";
    }
} // Metal
