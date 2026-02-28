#include "PassesService.h"
#include "../../../context/ApplicationContext.h"
#include "./CommandBufferRecorder.h"
#include "../compute-pass/impl/AccumulationPass.h"
#include "../compute-pass/impl/HWRayTracingPass.h"
#include "../render-pass/impl/GBufferGenPass.h"
#include "../render-pass/impl/PostProcessingPass.h"
#include "../render-pass/impl/tools/SelectedDotPass.h"
#include "../render-pass/impl/tools/GridPass.h"
#include "../render-pass/impl/tools/IconsPass.h"

namespace Metal {
    PassesService::PassesService() : AbstractRuntimeComponent() {
    }

    void PassesService::onInitialize() {
        gBuffer = new CommandBufferRecorder(CTX.coreFrameBuffers.gBufferFBO);
        compute = new CommandBufferRecorder();
        postProcessing = new CommandBufferRecorder(CTX.coreFrameBuffers.postProcessingFBO);

        addPass(gBufferPasses, new GBufferGenPass());

        addPass(computePasses, new HWRayTracingPass());
        addPass(computePasses, new AccumulationPass());

        addPass(postProcessingPasses, new PostProcessingPass());
        if (CTX.isDebugMode()) {
            addPass(postProcessingPasses, new SelectedDotPass());
            addPass(postProcessingPasses, new GridPass());
            addPass(postProcessingPasses, new IconsPass());
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
        postProcessing->recordCommands(postProcessingPasses);
    }

    void PassesService::dispose() {
        delete gBuffer;
        delete compute;
        delete postProcessing;

        computePasses.clear();
        gBufferPasses.clear();
        postProcessingPasses.clear();
        CTX.pipelineService.disposeAll();

        for (auto &pass: allPasses) {
            delete pass;
        }
        allPasses.clear();
    }
} // Metal
