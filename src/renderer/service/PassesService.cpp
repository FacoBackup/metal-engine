#include "PassesService.h"
#include "../../MetalContextProvider.h"
#include "../passes/CommandBufferRecorder.h"
#include "../passes/impl/AccumulationPass.h"
#include "../passes/impl/AccumulationMetadataPass.h"
#include "../passes/impl/RTPass.h"
#include "../passes/impl/PostProcessingPass.h"
#include "../passes/impl/tools/GridPass.h"
#include "../passes/impl/tools/IconsPass.h"
#include "../passes/impl/ui/UIPass.h"

namespace Metal {
    void PassesService::onInitialize() {
        compute = new CommandBufferRecorder();
        postProcessing = new CommandBufferRecorder(SINGLETONS.coreFrameBuffers.postProcessingFBO);

        addPass(computePasses, new RTPass());
        addPass(computePasses, new AccumulationPass());
        addPass(computePasses, new AccumulationMetadataPass());
        addPass(computePasses, new PostProcessingPass());

        if (ContextProvider::Get().isDebugMode()) {
            addPass(computePasses, new UIPass());
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
        compute->recordCommands(computePasses);
        postProcessing->recordCommands(postProcessingPasses);
    }

    void PassesService::dispose() {
        delete compute;
        delete postProcessing;

        computePasses.clear();
        postProcessingPasses.clear();

        for (auto &pass: allPasses) {
            pass->getPipeline()->dispose();
            delete pass;
        }
        allPasses.clear();
    }
} // Metal
