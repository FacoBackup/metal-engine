#include "PassesService.h"
#include "../../../context/ApplicationContext.h"
#include "./CommandBufferRecorder.h"
#include "../compute-pass/impl/GBufferShadingPass.h"
#include "../render-pass/impl/DenoiserPass.h"
#include "../render-pass/impl/GBufferGenPass.h"
#include "../render-pass/impl/PostProcessingPass.h"
#include "../render-pass/impl/tools/GridPass.h"
#include "../render-pass/impl/tools/VoxelVisualizerPass.h"
#include "../render-pass/impl/tools/IconsPass.h"

namespace Metal {
    PassesService::PassesService(ApplicationContext &context) : AbstractRuntimeComponent(context) {
    }

    void PassesService::onInitialize() {
        gBuffer = new CommandBufferRecorder(context.coreFrameBuffers.gBufferFBO, context);
        compute = new CommandBufferRecorder(context);
        postProcessing = new CommandBufferRecorder(context.coreFrameBuffers.postProcessingFBO, context);
        denoising = new CommandBufferRecorder(context.coreFrameBuffers.denoisedResultFBO, context);

        addPass(gBufferPasses, new GBufferGenPass(context));

        addPass(computePasses, new GBufferShadingPass(context));
        addPass(denoisingPass, new DenoiserPass(context));
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
        p.push_back(pointer);
        allPasses.push_back(pointer);
    }

    void PassesService::onSync() {
        gBuffer->recordCommands(gBufferPasses);
        compute->recordCommands(computePasses);
        denoising->recordCommands(denoisingPass);
        postProcessing->recordCommands(postProcessingPasses);
    }

    void PassesService::dispose() {
        delete gBuffer;
        delete compute;
        delete postProcessing;
        delete denoising;

        computePasses.clear();
        denoisingPass.clear();
        gBufferPasses.clear();
        postProcessingPasses.clear();

        for (auto &pass: allPasses) {
            pass->getPipeline()->dispose(context.vulkanContext);
            delete pass;
        }
        allPasses.clear();
    }
} // Metal
