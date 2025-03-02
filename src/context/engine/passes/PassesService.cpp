#include "PassesService.h"
#include "../../../context/ApplicationContext.h"
#include "./CommandBufferRecorder.h"
#include "../compute-pass/impl/AccumulationPass.h"
#include "../compute-pass/impl/AccumulationMetadataPass.h"
#include "../compute-pass/impl/RTPass.h"
#include "../render-pass/impl/PostProcessingPass.h"
#include "../render-pass/impl/tools/GridPass.h"
#include "../render-pass/impl/tools/IconsPass.h"

namespace Metal {
    PassesService::PassesService(ApplicationContext &context) : AbstractRuntimeComponent(context) {
    }

    void PassesService::onInitialize() {
        compute = new CommandBufferRecorder(context);
        postProcessing = new CommandBufferRecorder(context.coreFrameBuffers.postProcessingFBO, context);

        addPass(computePasses, new RTPass(context));
        addPass(computePasses, new AccumulationPass(context));
        addPass(computePasses, new AccumulationMetadataPass(context));

        addPass(postProcessingPasses, new PostProcessingPass(context));

        if (context.isDebugMode()) {
            addPass(postProcessingPasses, new GridPass(context));
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
        compute->recordCommands(computePasses);
        postProcessing->recordCommands(postProcessingPasses);
    }

    void PassesService::dispose() {
        delete compute;
        delete postProcessing;

        computePasses.clear();
        postProcessingPasses.clear();

        for (auto &pass: allPasses) {
            pass->getPipeline()->dispose(context.vulkanContext);
            delete pass;
        }
        allPasses.clear();
    }
} // Metal
