#include "PostProcessingPass.h"

#include "../../../MetalContextProvider.h"
#include "../../data/PipelineBuilder.h"
#include "../../data/TextureInstance.h"

namespace Metal {
    void PostProcessingPass::onInitialize() {
        PipelineBuilder ppPipelineBuilder = PipelineBuilder::Of("PostProcessing.comp")
                .setPushConstantsSize(sizeof(PostProcessingPC))
                .addDescriptorSet(SINGLETONS.coreDescriptorSets.current1FrameDescriptor.get())
                .addDescriptorSet(SINGLETONS.coreDescriptorSets.current2FrameDescriptor.get());
        pipelineInstance = SINGLETONS.pipelineService.createPipeline(ppPipelineBuilder);
    }

    void PostProcessingPass::onSync() {
        auto &camera = SINGLETONS.worldRepository.camera;
        pushConstant.distortionIntensity = camera.distortionIntensity;
        pushConstant.chromaticAberrationIntensity = camera.chromaticAberrationIntensity;
        pushConstant.distortionEnabled = camera.distortionEnabled;
        pushConstant.chromaticAberrationEnabled = camera.chromaticAberrationEnabled;
        pushConstant.vignetteEnabled = camera.vignetteEnabled;
        pushConstant.vignetteStrength = camera.vignetteStrength;

        recordPushConstant(&pushConstant);
        startWriting(SINGLETONS.coreTextures.current2Frame->vkImage);
        recordImageDispatch(SINGLETONS.coreTextures.current1Frame, 8, 8);
        endWriting(SINGLETONS.coreTextures.current2Frame->vkImage);
    }

} // Metal
