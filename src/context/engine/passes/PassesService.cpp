#include "PassesService.h"
#include "../../../context/ApplicationContext.h"
#include "./CommandBufferRecorder.h"
#include "../render-pass/impl/GBufferShadingPass.h"
#include "../render-pass/impl/GBufferGenPass.h"
#include "../render-pass/impl/PostProcessingPass.h"
#include "../render-pass/impl/tools/GridPass.h"
#include "../render-pass/impl/AtmospherePass.h"
#include "../compute-pass/impl/GlobalIlluminationPass.h"
#include "../render-pass/impl/tools/VoxelVisualizerPass.h"
#include "../render-pass/impl/tools/IconsPass.h"

namespace Metal {
    PassesService::PassesService(ApplicationContext &context) : AbstractRuntimeComponent(context) {
    }

    void PassesService::onInitialize() {
        gBuffer = new CommandBufferRecorder(context.coreFrameBuffers.gBufferFBO, context);
        fullScreen = new CommandBufferRecorder(context.coreFrameBuffers.shadingFBO, context);
        postProcessing = new CommandBufferRecorder(context.coreFrameBuffers.postProcessingFBO, context);

        addPass(fullScreenRenderPasses, new GBufferShadingPass(context));
        addPass(fullScreenRenderPasses, new AtmospherePass(context));
        if (context.isDebugMode()) {
            addPass(fullScreenRenderPasses, new GridPass(context));
            addPass(fullScreenRenderPasses, new VoxelVisualizerPass(context));
            addPass(fullScreenRenderPasses, new IconsPass(context));
        }
        addPass(computePasses, new GlobalIlluminationPass(context));
        addPass(postProcessingPasses, new PostProcessingPass(context));
        addPass(gBufferPasses, new GBufferGenPass(context));

        for (auto *pass : allPasses) {
            pass->onInitialize();
        }
    }

    void PassesService::addPass(std::vector<AbstractPass *> &p, AbstractPass *pointer) {
        p.push_back(std::move(pointer));
        allPasses.push_back(pointer);
    }

    void PassesService::onSync() {
        gBuffer->recordCommands(gBufferPasses);
        fullScreen->recordCommands(fullScreenRenderPasses, computePasses);
        postProcessing->recordCommands(postProcessingPasses);
    }

    void PassesService::dispose() {
        delete gBuffer;
        delete fullScreen;
        delete postProcessing;


        computePasses.clear();
        giPasses.clear();
        fullScreenRenderPasses.clear();
        gBufferPasses.clear();
        postProcessingPasses.clear();

        for (auto &pass: allPasses) {
            pass->getPipeline()->dispose(context.vulkanContext);

            delete pass;
        }
        allPasses.clear();
    }
} // Metal
