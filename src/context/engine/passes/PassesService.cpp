#include "PassesService.h"
#include "../../../context/ApplicationContext.h"
#include "./CommandBufferRecorder.h"
#include "../render-pass/impl/GBufferShadingPass.h"
#include "../render-pass/impl/OpaqueRenderPass.h"
#include "../render-pass/impl/PostProcessingPass.h"
#include "../render-pass/impl/tools/GridPass.h"
#include "../render-pass/impl/AtmospherePass.h"
#include "../render-pass/impl/GlobalIlluminationPass.h"
#include "../render-pass/impl/tools/VoxelVisualizerPass.h"
#include "../render-pass/impl/tools/IconsPass.h"

namespace Metal {
    PassesService::PassesService(ApplicationContext &context) : AbstractRuntimeComponent(context) {
    }

    void PassesService::onInitialize() {
        context.worldGridService.onSync();
        fullScreenRenderPasses.push_back(std::make_unique<GBufferShadingPass>(context));
        fullScreenRenderPasses.push_back(std::make_unique<AtmospherePass>(context));
        if (context.isDebugMode()) {
            fullScreenRenderPasses.push_back(std::make_unique<GridPass>(context));
            fullScreenRenderPasses.push_back(std::make_unique<VoxelVisualizerPass>(context));
            fullScreenRenderPasses.push_back(std::make_unique<IconsPass>(context));
        }
        aoPass.push_back(std::make_unique<GlobalIlluminationPass>(context));
        postProcessingPasses.push_back(std::make_unique<PostProcessingPass>(context));
        gBufferPasses.push_back(std::make_unique<OpaqueRenderPass>(context));
    }

    void PassesService::onSync() {
        context.coreRenderPasses.aoPass->recordCommands(aoPass);
        context.coreRenderPasses.gBufferPass->recordCommands(gBufferPasses);
        context.coreRenderPasses.fullScreenPass->recordCommands(fullScreenRenderPasses);
        context.coreRenderPasses.postProcessingPass->recordCommands(postProcessingPasses);
    }
} // Metal
