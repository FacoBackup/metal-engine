#include "EngineRepository.h"

#include "../../common/Icons.h"
#include "../../ApplicationContext.h"
#include "../../common/FileExtensions.h"

namespace Metal {
    void EngineRepository::clear() {
        // Nothing to clear
    }

    void EngineRepository::registerFields() {
        registerEditableField<INT>(&shadingResInvScale).setName("shadingResInvScale").setLabel("Shading inverted resolution scale").setMin(1).setMax(16).setIncrement(1);
        registerEditableField<BOOLEAN>(&vsync).setName("vsync").setLabel("VSync enabled");

        registerEditableField<FLOAT>(&pathTracerMultiplier).setName("pathTracerMultiplier").setLabel("Path tracer multiplier").setMin(0).setMax(100).setIncrement(0.1f);
        registerEditableField<BOOLEAN>(&denoiserEnabled).setName("denoiserEnabled").setLabel("Denoiser enabled");
        registerEditableField<BOOLEAN>(&isRayTracingEnabled).setName("isRayTracingEnabled").setLabel("Ray tracing enabled");
        registerEditableField<INT>(&pathTracerMaxSamples).setName("pathTracerMaxSamples").setLabel("Path tracer max accumulation").setMin(1).setMax(10000).setIncrement(10);
        registerEditableField<INT>(&pathTracerSamples).setName("pathTracerSamples").setLabel("Path tracer samples per pixel").setMin(1).setMax(32).setIncrement(1);
        registerEditableField<INT>(&pathTracerBounces).setName("pathTracerBounces").setLabel("Path tracer bounces").setMin(0).setMax(16).setIncrement(1);
        registerEditableField<FLOAT>(&pathTracingEmissiveFactor).setName("pathTracingEmissiveFactor").setLabel("Path tracing emissive factor").setMin(0).setMax(100).setIncrement(0.1f);

        registerEditableField<BOOLEAN>(&atmosphereEnabled).setName("atmosphereEnabled").setLabel("Atmosphere enabled");
        registerEditableField<BOOLEAN>(&restirEnabled).setName("restirEnabled").setLabel("ReSTIR GI enabled");
        registerEditableField<FLOAT>(&elapsedTime).setName("elapsedTime").setLabel("Atmosphere elapsed time").setMin(0).setMax(24).setIncrement(0.01f);

        registerEditableField<VECTOR3>(&gravity).setName("gravity").setLabel("Physics gravity").setIncrement(0.1f);
        registerEditableField<RESOURCE>(&mainLevel).setName("mainLevel").setLabel("Main level").setSupportedFileTypes({FileExtensions::level.get()});
    }


    const char *EngineRepository::getIcon() const {
        return Icons::display_settings.c_str();
    }

    const char *EngineRepository::getTitle() const {
        return "Engine settings";
    }
} // Metal
