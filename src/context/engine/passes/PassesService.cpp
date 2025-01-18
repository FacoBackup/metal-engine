#include "PassesService.h"
#include "../../../context/ApplicationContext.h"
#include "./CommandBufferRecorder.h"
#include "../compute-pass/impl/GBufferShadingPass.h"
#include "../render-pass/impl/GBufferGenPass.h"
#include "../render-pass/impl/PostProcessingPass.h"
#include "../render-pass/impl/tools/GridPass.h"
#include "../compute-pass/impl/GlobalIlluminationPass.h"
#include "../render-pass/impl/tools/VoxelVisualizerPass.h"
#include "../render-pass/impl/tools/IconsPass.h"

namespace Metal {
    PassesService::PassesService(ApplicationContext &context) : AbstractRuntimeComponent(context) {
    }

    void PassesService::onInitialize() {
        gBuffer = new CommandBufferRecorder(context.coreFrameBuffers.gBufferFBO, context);
        compute = new CommandBufferRecorder( context);
        postProcessing = new CommandBufferRecorder(context.coreFrameBuffers.postProcessingFBO, context);

        addPass(gBufferPasses, new GBufferGenPass(context));

        addPass(computePasses, new GlobalIlluminationPass(context));
        addPass(computePasses, new GBufferShadingPass(context));
        addPass(postProcessingPasses, new PostProcessingPass(context));
        if (context.isDebugMode()) {
            addPass(postProcessingPasses, new GridPass(context));
            addPass(postProcessingPasses, new VoxelVisualizerPass(context));
            addPass(postProcessingPasses, new IconsPass(context));
        }

        for (auto *pass: allPasses) {
            pass->onInitialize();
        }
    }

    void PassesService::addPass(std::vector<AbstractPass *> &p, AbstractPass *pointer) {
        p.push_back(std::move(pointer));
        allPasses.push_back(pointer);
    }

    void PassesService::onSync() {
        gBuffer->recordCommands(gBufferPasses);
        compute->recordCommands(computePasses);
        postProcessing->recordCommands(postProcessingPasses);
    }

    void PassesService::dispose() {
        delete gBuffer;
        delete compute;
        delete postProcessing;


        computePasses.clear();
        giPasses.clear();
        gBufferPasses.clear();
        postProcessingPasses.clear();

        for (auto &pass: allPasses) {
            pass->getPipeline()->dispose(context.vulkanContext);

            delete pass;
        }
        allPasses.clear();
    }
} // Metal
