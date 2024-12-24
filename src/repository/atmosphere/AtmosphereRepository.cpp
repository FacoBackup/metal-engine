#include "AtmosphereRepository.h"

#include "../../common/interface/Icons.h"

namespace Metal {
    void AtmosphereRepository::registerFields() {
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

    const char * AtmosphereRepository::getIcon() {
        return Icons::cloud_sync.c_str();
    }

    const char * AtmosphereRepository::getTitle() {
        return "Atmosphere";
    }
} // Metal
