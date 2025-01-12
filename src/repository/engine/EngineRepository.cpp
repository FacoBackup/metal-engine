#include "EngineRepository.h"

#include "../../common/interface/Icons.h"
#include "../../context/ApplicationContext.h"

#define LEVEL_OF_DETAIL "Level of detail"
#define GLOBAL_ILLUMINATION "Global illumination"
#define ATMOSPHERE "Atmosphere"

namespace Metal {
    void EngineRepository::registerFields() {
        registerInt(shadingResInvScale, "", "Shading inverted resolution scale (Restart required)", 1, 16);
        registerInt(numberOfTiles, "World", "Number of tiles", 2, 100);
        registerBool(giEnabled, GLOBAL_ILLUMINATION, "Enabled?");
        registerFloat(giStrength, GLOBAL_ILLUMINATION, "Strength");
        registerInt(giBounces, GLOBAL_ILLUMINATION, "Max bounces", 0, 5);
        registerInt(giTileSubdivision, GLOBAL_ILLUMINATION, "Grid subdivision", 1);
        registerFloat(giEmissiveFactor, GLOBAL_ILLUMINATION, "Emissive surface factor", 0);
        registerBool(atmosphereEnabled, ATMOSPHERE, "Enabled?");
        registerFloat(elapsedTime, ATMOSPHERE, "Elapsed time");
        registerBool(incrementTime, ATMOSPHERE, "Increment time");
        registerFloat(elapsedTimeSpeed, ATMOSPHERE, "Time of day speed");
        registerFloat(sunDistance, ATMOSPHERE, "Sun distance");
        registerFloat(sunLightIntensity, ATMOSPHERE, "Sun light intensity");
        registerColor(dawnColor, ATMOSPHERE, "Dawn color");
        registerColor(nightColor, ATMOSPHERE, "Night color");
        registerColor(middayColor, ATMOSPHERE, "Midday color");
        registerBool(screenSpaceShadows, ATMOSPHERE, "Screen space shadows");
    }

    void EngineRepository::onUpdate(InspectableMember *member, ApplicationContext &context) {
        if (member != nullptr && member->name == LEVEL_OF_DETAIL) {
            context.worldGridRepository.hasMainTileChanged = true;
        }
        if (member != nullptr && (member->group == GLOBAL_ILLUMINATION || member->group == ATMOSPHERE)) {
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
