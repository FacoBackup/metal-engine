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
    void EngineRepository::clear() {
        // Nothing to clear
    }

    void EngineRepository::registerFields() {
        registerEditableField<INT>(&shadingResInvScale).setName("Shading inverted resolution scale").setGroup("Display settings (Restart required)").setMin(1).setMax(16);
        registerEditableField<BOOLEAN>(&vsync).setName("VSync?").setGroup("Display settings (Restart required)");

        registerEditableField<INT>(&volumeShadowSteps).setName("Shadow steps").setGroup(VOLUMES).setMin(1);

        registerEditableField<FLOAT>(&pathTracerMultiplier).setName("Strength").setGroup(PATH_TRACER);
        registerEditableField<BOOLEAN>(&denoiserEnabled).setName("Enable denoiser?").setGroup(PATH_TRACER);
        registerEditableField<INT>(&pathTracerMaxSamples).setName("Maximum accumulation").setGroup(PATH_TRACER).setMin(1).setMax(10000);
        registerEditableField<INT>(&pathTracerSamples).setName("Samples per pixel").setGroup(PATH_TRACER).setMin(1).setMax(32);
        registerEditableField<INT>(&pathTracerBounces).setName("Bounces").setGroup(PATH_TRACER).setMin(0).setMax(7);
        registerEditableField<FLOAT>(&pathTracingEmissiveFactor).setName("Emissive surface factor").setGroup(PATH_TRACER).setMin(0);

        registerEditableField<BOOLEAN>(&dofEnabled).setName("Enable depth of field?").setGroup(DEPTH_OF_FIELD);
        registerEditableField<FLOAT>(&dofFocusDistance).setName("Focus distance").setGroup(DEPTH_OF_FIELD);
        registerEditableField<FLOAT>(&dofAperture).setName("Aperture").setGroup(DEPTH_OF_FIELD);
        registerEditableField<FLOAT>(&dofFocalLength).setName("Focal length").setGroup(DEPTH_OF_FIELD);

        registerEditableField<BOOLEAN>(&atmosphereEnabled).setName("Enable atmosphere?").setGroup(ATMOSPHERE);
        registerEditableField<FLOAT>(&elapsedTime).setName("Elapsed time").setGroup(ATMOSPHERE);

        registerEditableField<VECTOR3>(&gravity).setName("Gravity").setGroup(PHYSICS);
    }


    const char *EngineRepository::getIcon() const {
        return Icons::display_settings.c_str();
    }

    const char *EngineRepository::getTitle() const {
        return "Engine settings";
    }
} // Metal
