#include "EngineRepository.h"

#include "../../common/Icons.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void EngineRepository::clear() {
        // Nothing to clear
    }

    void EngineRepository::registerFields() {
        registerEditableField<INT>(&shadingResInvScale).setName("shadingResInvScale").setLabel("Shading inverted resolution scale").setMin(1).setMax(16);
        registerEditableField<BOOLEAN>(&vsync).setName("vsync").setLabel("VSync enabled");

        registerEditableField<FLOAT>(&pathTracerMultiplier).setName("pathTracerMultiplier").setLabel("Path tracer multiplier");
        registerEditableField<BOOLEAN>(&denoiserEnabled).setName("denoiserEnabled").setLabel("Denoiser enabled");
        registerEditableField<INT>(&pathTracerMaxSamples).setName("pathTracerMaxSamples").setLabel("Path tracer max accumulation").setMin(1).setMax(10000);
        registerEditableField<INT>(&pathTracerSamples).setName("pathTracerSamples").setLabel("Path tracer samples per pixel").setMin(1).setMax(32);
        registerEditableField<INT>(&pathTracerBounces).setName("pathTracerBounces").setLabel("Path tracer bounces").setMin(0).setMax(7);
        registerEditableField<FLOAT>(&pathTracingEmissiveFactor).setName("pathTracingEmissiveFactor").setLabel("Path tracing emissive factor").setMin(0);

        registerEditableField<BOOLEAN>(&dofEnabled).setName("dofEnabled").setLabel("Depth of field enabled");
        registerEditableField<FLOAT>(&dofFocusDistance).setName("dofFocusDistance").setLabel("Depth of field focus distance");
        registerEditableField<FLOAT>(&dofAperture).setName("dofAperture").setLabel("Depth of field aperture");
        registerEditableField<FLOAT>(&dofFocalLength).setName("dofFocalLength").setLabel("Depth of field focal length");

        registerEditableField<BOOLEAN>(&atmosphereEnabled).setName("atmosphereEnabled").setLabel("Atmosphere enabled");
        registerEditableField<FLOAT>(&elapsedTime).setName("elapsedTime").setLabel("Atmosphere elapsed time");

        registerEditableField<VECTOR3>(&gravity).setName("gravity").setLabel("Physics gravity");
    }


    const char *EngineRepository::getIcon() const {
        return Icons::display_settings.c_str();
    }

    const char *EngineRepository::getTitle() const {
        return "Engine settings";
    }
} // Metal
