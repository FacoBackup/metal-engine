#include "EngineRepository.h"

#include "../../common/Icons.h"
#include "../../ApplicationContext.h"

#define PATH_TRACER "Path tracer"
#define ATMOSPHERE "Atmosphere"
#define SUN "Sun"
#define VOLUMES "Volumes"
#define DEPTH_OF_FIELD "Depth of field"
#define PHYSICS "Physics"

namespace Metal {
    void EngineRepository::registerFields() {
        registerEditableField(&shadingResInvScale, INT, "Shading inverted resolution scale", "Display settings (Restart required)").setMin(1).setMax(16);
        registerEditableField(&vsync, BOOLEAN, "VSync?", "Display settings (Restart required)");

        registerEditableField(&volumeShadowSteps, INT, "Shadow steps", VOLUMES).setMin(1);

        registerEditableField(&pathTracerMultiplier, FLOAT, "Strength", PATH_TRACER);
        registerEditableField(&denoiserEnabled, BOOLEAN, "Enable denoiser?", PATH_TRACER);
        registerEditableField(&pathTracerMaxSamples, INT, "Maximum accumulation", PATH_TRACER).setMin(1).setMax(10000);
        registerEditableField(&pathTracerSamples, INT, "Samples per pixel", PATH_TRACER).setMin(1).setMax(32);
        registerEditableField(&pathTracerBounces, INT, "Bounces", PATH_TRACER).setMin(0).setMax(7);
        registerEditableField(&pathTracingEmissiveFactor, FLOAT, "Emissive surface factor", PATH_TRACER).setMin(0);

        registerEditableField(&dofEnabled, BOOLEAN, "Enable depth of field?", DEPTH_OF_FIELD);
        registerEditableField(&dofFocusDistance, FLOAT, "Focus distance", DEPTH_OF_FIELD);
        registerEditableField(&dofAperture, FLOAT, "Aperture", DEPTH_OF_FIELD);
        registerEditableField(&dofFocalLength, FLOAT, "Focal length", DEPTH_OF_FIELD);

        registerEditableField(&atmosphereEnabled, BOOLEAN, "Enable atmosphere?", ATMOSPHERE);
        registerEditableField(&elapsedTime, FLOAT, "Elapsed time", ATMOSPHERE);

        registerEditableField(&gravity, VECTOR3, "Gravity", PHYSICS);
    }


    const char *EngineRepository::getIcon() const {
        return Icons::display_settings.c_str();
    }

    const char *EngineRepository::getTitle() const {
        return "Engine settings";
    }
} // Metal
