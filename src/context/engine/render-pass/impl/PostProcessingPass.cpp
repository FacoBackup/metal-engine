#include "PostProcessingPass.h"

#include "../../../../context/ApplicationContext.h"

namespace Metal {
    void PostProcessingPass::onSync() {
        auto &camera = context.worldRepository.camera;
        pushConstant.distortionIntensity = camera.distortionIntensity;
        pushConstant.chromaticAberrationIntensity = camera.chromaticAberrationIntensity;
        pushConstant.distortionEnabled = camera.distortionEnabled;
        pushConstant.chromaticAberrationEnabled = camera.chromaticAberrationEnabled;
        pushConstant.vignetteEnabled = camera.vignetteEnabled;
        pushConstant.vignetteStrength = camera.vignetteStrength;

        recordDrawSimpleInstanced(3, 1);
    }

    PipelineInstance *PostProcessingPass::getPipeline() {
        return context.corePipelines.postProcessingPipeline;
    }
} // Metal
