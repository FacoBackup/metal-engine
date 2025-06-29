#include "EngineRepository.h"

#include "../../common/enum/Icons.h"
#include "../../MetalContextProvider.h"

#define PATH_TRACER "Path tracer"
#define ATMOSPHERE "Atmosphere"
#define SUN "Sun"
#define VOLUMES "Volumes"

namespace Metal {
    void EngineRepository::registerFields() {
        registerInt(shadingResInvScale, "Display settings (Restart required)", "Shading inverted resolution scale", 1,
                    16);
        registerBool(vsync, "Display settings (Restart required)", "VSync?");
        registerInt(volumeShadowSteps, VOLUMES, "Shadow steps", 1);

        registerBool(enabledDenoiser, PATH_TRACER, "Enable denoiser?");
        registerFloat(denoiserDiffWeight, PATH_TRACER, "Denoiser variance weight", 0, 10, false, .001);
        registerFloat(giStrength, PATH_TRACER, "Strength");
        registerInt(maxAccumulation, PATH_TRACER, "Maximum accumulation", 1, 10000);
        registerInt(giSamples, PATH_TRACER, "Samples per pixel", 1, 32);
        registerInt(giBounces, PATH_TRACER, "Bounces", 0, 7);
        registerBool(multipleImportanceSampling, PATH_TRACER, "Enable multiple importance sampling?");
        registerInt(giTileSubdivision, PATH_TRACER, "Grid subdivision", 1);
        registerInt(surfaceCacheMinSamples, PATH_TRACER, "Surface cache min samples", 1);
        registerFloat(giEmissiveFactor, PATH_TRACER, "Emissive surface factor", 0);

        registerBool(atmosphereEnabled, ATMOSPHERE, "Enabled?");
        registerFloat(elapsedTime, ATMOSPHERE, "Elapsed time");
        registerBool(incrementTime, ATMOSPHERE, "Increment time");
        registerFloat(elapsedTimeSpeed, ATMOSPHERE, "Time of day speed");
        registerFloat(sunDistance, ATMOSPHERE, "Sun distance");
        registerFloat(sunRadius, ATMOSPHERE, "Sun radius");
        registerFloat(sunLightIntensity, ATMOSPHERE, "Sun light intensity");
        registerColor(dawnColor, SUN, "Dawn color");
        registerColor(nightColor, SUN, "Night color");
        registerColor(middayColor, SUN, "Midday color");
    }

    void EngineRepository::onUpdate(InspectableMember *member) {
        if (member != nullptr && (member->group == PATH_TRACER || member->group == ATMOSPHERE || member->group
                                  == SUN)) {
            SINGLETONS.engineContext.setGISettingsUpdated(true);
            SINGLETONS.engineContext.setLightVolumeDataNeedsUpdate(true);
        }
    }

    const char *EngineRepository::getIcon() {
        return Icons::display_settings.c_str();
    }

    const char *EngineRepository::getTitle() {
        return "Engine settings";
    }
} // Metal
