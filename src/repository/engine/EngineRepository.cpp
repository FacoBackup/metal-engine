#include "EngineRepository.h"

#include "../../common/interface/Icons.h"
#include "../../context/ApplicationContext.h"

#define LEVEL_OF_DETAIL "Level of detail"
#define GLOBAL_ILLUMINATION "Global illumination"

namespace Metal {
    void EngineRepository::registerFields() {
        registerBool(atmosphereEnabled, "Atmosphere", "Enabled?");

        registerBool(giEnabled, GLOBAL_ILLUMINATION, "Enabled?");
        registerFloat(giStrength, GLOBAL_ILLUMINATION, "Strength");
        registerInt(giResScale, GLOBAL_ILLUMINATION, "Inverted resolution scale (Restart required)", 1, 16);
        registerInt(giBounces, GLOBAL_ILLUMINATION, "Max bounces", 0, 5);
        registerInt(giTileSubdivision, GLOBAL_ILLUMINATION, "Grid subdivision", 1);
        registerInt(numberOfTiles, "World", "Number of tiles", 2, 100);
        registerFloat(elapsedTime, "Time", "Elapsed time");
        registerBool(incrementTime, "Time", "Increment time");
        registerFloat(elapsedTimeSpeed, "Time", "Time of day speed");
        registerFloat(sunDistance, "Sun", "Sun distance");
        registerFloat(sunLightIntensity, "Sun", "Light intensity");
        registerColor(dawnColor, "Sun", "Dawn color");
        registerColor(nightColor, "Sun", "Night color");
        registerColor(middayColor, "Sun", "Midday color");
        registerBool(screenSpaceShadows, "Sun", "Screen space shadows");
    }

    void EngineRepository::onUpdate(InspectableMember *member, ApplicationContext &context) {
        if (member != nullptr && member->name == LEVEL_OF_DETAIL) {
            context.worldGridRepository.hasMainTileChanged = true;
        }
        if (member != nullptr && member->group == GLOBAL_ILLUMINATION) {
            context.engineContext.setGISettingsUpdated(true);
        }
    }

    const char *EngineRepository::getIcon() {
        return Icons::display_settings.c_str();
    }

    const char *EngineRepository::getTitle() {
        return "Engine settings";
    }
} // Metal
