#include "UIPass.h"
#include "../../../../MetalContextProvider.h"
#include "../../../data/TextureInstance.h"
#include "../../../data/PipelineBuilder.h"

namespace Metal {
    void UIPass::onInitialize() {
        PipelineBuilder ppPipelineBuilder = PipelineBuilder::Of("ui/UserInterface.comp")
                .setPushConstantsSize(sizeof(UserInterfacePC))
                .addDescriptorSet(SINGLETONS.coreDescriptorSets.current2FrameDescriptor.get())
                .addDescriptorSet(SINGLETONS.coreDescriptorSets.current1FrameDescriptor.get())
                .addDescriptorSet(SINGLETONS.coreDescriptorSets.windowDescriptor.get());
        pipelineInstance = SINGLETONS.pipelineService.createPipeline(ppPipelineBuilder);
    }

    void UIPass::onSync() {
        pushConstant.windowCount = SINGLETONS.windowService.getCurrentWindow();
        pushConstant.blurStrength = 5;
        pushConstant.isDarkMode = SINGLETONS.editorRepository.isDarkMode;
        pushConstant.isCompact = false;
        pushConstant.borderRadius = 8;
        pushConstant.shadowSize = 12;
        pushConstant.viewportSize.x = SINGLETONS.runtimeRepository.viewportW;
        pushConstant.viewportSize.y = SINGLETONS.runtimeRepository.viewportH;
        pushConstant.viewportPos.x = SINGLETONS.runtimeRepository.viewportX;
        pushConstant.viewportPos.y = SINGLETONS.runtimeRepository.viewportY;
        pushConstant.windowAspectRatio.x = SINGLETONS.runtimeRepository.viewportW / SINGLETONS.coreTextures.current1Frame->width;
        pushConstant.windowAspectRatio.y = SINGLETONS.runtimeRepository.viewportH / SINGLETONS.coreTextures.current1Frame->height;

        recordPushConstant(&pushConstant);

        startWriting(SINGLETONS.coreTextures.current1Frame->vkImage);
        recordImageDispatch(SINGLETONS.coreTextures.current1Frame, 8, 8);
        endWriting(SINGLETONS.coreTextures.current1Frame->vkImage);
    }
} // Metal
