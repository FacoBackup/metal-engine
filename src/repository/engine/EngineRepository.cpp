#include "EngineRepository.h"

#include "../../common/interface/Icons.h"
#include "../../context/ApplicationContext.h"

#define LEVEL_OF_DETAIL "Level of detail"
#define GLOBAL_ILLUMINATION "Global illumination"
#define DENOISING "Denoising"
#define ATMOSPHERE "Atmosphere"
#define SUN "Sun"

namespace Metal {
    void EngineRepository::registerFields() {
        registerInt(shadingResInvScale, "Display settings (Restart required)", "Shading inverted resolution scale", 1,
                    16);
        registerBool(vsync, "Display settings (Restart required)", "VSync?");
        registerInt(numberOfTiles, "World", "Number of tiles", 2, 100);
        registerFloat(giStrength, GLOBAL_ILLUMINATION, "Strength");
        registerInt(giMaxAccumulation, GLOBAL_ILLUMINATION, "Maximum accumulation", 1, 10000);
        registerInt(giSamples, GLOBAL_ILLUMINATION, "Samples per pixel", 1, 32);
        registerInt(giBounces, GLOBAL_ILLUMINATION, "Bounces", 0, 7);
        registerBool(multipleImportanceSampling, GLOBAL_ILLUMINATION, "Enable multiple importance sampling?");
        registerInt(giTileSubdivision, GLOBAL_ILLUMINATION, "Grid subdivision", 1);
        registerFloat(giEmissiveFactor, GLOBAL_ILLUMINATION, "Emissive surface factor", 0);

        registerBool(enabledDenoiser, DENOISING, "Enabled?");
        registerFloat(denoiserStepWidth, DENOISING, "Step width", 1, 15, false, .001);
        registerFloat(denoiserNormalPhi, DENOISING, "Normal weight", 0, 1, false, .001);
        registerFloat(denoiserNoiseThreshold, DENOISING, "Noise threshold", 0);

        registerBool(atmosphereEnabled, ATMOSPHERE, "Enabled?");
        registerFloat(elapsedTime, ATMOSPHERE, "Elapsed time");
        registerBool(incrementTime, ATMOSPHERE, "Increment time");
        registerFloat(elapsedTimeSpeed, ATMOSPHERE, "Time of day speed");
        registerFloat(sunDistance, SUN, "Sun distance");
        registerFloat(sunRadius, SUN, "Sun radius");
        registerFloat(sunLightIntensity, SUN, "Sun light intensity");
        registerColor(dawnColor, SUN, "Dawn color");
        registerColor(nightColor, SUN, "Night color");
        registerColor(middayColor, SUN, "Midday color");
    }

    void EngineRepository::onUpdate(InspectableMember *member, ApplicationContext &context) {
        if (member != nullptr && member->name == LEVEL_OF_DETAIL) {
            context.worldGridRepository.hasMainTileChanged = true;
        }
        if (member != nullptr && (member->group == GLOBAL_ILLUMINATION || member->group == ATMOSPHERE || member->group
                                  == SUN)) {
            context.engineContext.setGISettingsUpdated(true);
            context.engineContext.setLightingDataUpdated(true);
        }
    }

    const char *EngineRepository::getIcon() {
        return Icons::display_settings.c_str();
    }

    const char *EngineRepository::getTitle() {
        return "Engine settings";
    }
} // Metal
