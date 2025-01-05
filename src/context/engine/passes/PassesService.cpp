#include "PassesService.h"
#include "../../../context/ApplicationContext.h"
#include "./CommandBufferRecorder.h"
#include "../render-pass/impl/GBufferShadingPass.h"
#include "../render-pass/impl/OpaqueRenderPass.h"
#include "../render-pass/impl/PostProcessingPass.h"
#include "../render-pass/impl/tools/GridPass.h"
#include "../render-pass/impl/RayGenPass.h"
#include "../render-pass/impl/AtmospherePass.h"
#include "../compute-pass/impl/GlobalIlluminationPass.h"
#include "../render-pass/impl/tools/VoxelVisualizerPass.h"
#include "../render-pass/impl/tools/IconsPass.h"

namespace Metal {
    PassesService::PassesService(ApplicationContext &context) : AbstractRuntimeComponent(context) {
    }

    void PassesService::onInitialize() {
        gBuffer = new CommandBufferRecorder(context.coreFrameBuffers.gBufferFBO, context);
        rayGen = new CommandBufferRecorder(context.coreFrameBuffers.rayGenFBO, context);
        fullScreen = new CommandBufferRecorder(context.coreFrameBuffers.auxFBO, context);
        postProcessing = new CommandBufferRecorder(context.coreFrameBuffers.postProcessingFBO, context);

        rayGenPass.push_back(std::make_unique<RayGenPass>(context));

        context.worldGridService.onSync();
        fullScreenRenderPasses.push_back(std::make_unique<GBufferShadingPass>(context));
        fullScreenRenderPasses.push_back(std::make_unique<AtmospherePass>(context));
        if (context.isDebugMode()) {
            fullScreenRenderPasses.push_back(std::make_unique<GridPass>(context));
            fullScreenRenderPasses.push_back(std::make_unique<VoxelVisualizerPass>(context));
            fullScreenRenderPasses.push_back(std::make_unique<IconsPass>(context));
        }
        computePasses.push_back(std::make_unique<GlobalIlluminationPass>(context));
        postProcessingPasses.push_back(std::make_unique<PostProcessingPass>(context));
        gBufferPasses.push_back(std::make_unique<OpaqueRenderPass>(context));
    }

    void PassesService::onSync() {
        if (context.engineRepository.giEnabled) {
            rayGen->recordCommands(rayGenPass);
        }
        gBuffer->recordCommands(gBufferPasses);
        fullScreen->recordCommands(fullScreenRenderPasses, computePasses);
        postProcessing->recordCommands(postProcessingPasses);
    }
} // Metal
