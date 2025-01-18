#ifndef PASSESSERVICE_H
#define PASSESSERVICE_H
#include <memory>
#include <vector>

#include "AbstractPass.h"
#include "../../../common/AbstractRuntimeComponent.h"


namespace Metal {
    class CommandBufferRecorder;

    class PassesService final : public AbstractRuntimeComponent {
        std::vector<AbstractPass *> computePasses;
        std::vector<AbstractPass *> giPasses;
        std::vector<AbstractPass *> fullScreenRenderPasses;
        std::vector<AbstractPass *> gBufferPasses;
        std::vector<AbstractPass *> postProcessingPasses;
        std::vector<AbstractPass *> allPasses;

        CommandBufferRecorder *gBuffer = nullptr;
        CommandBufferRecorder *fullScreen = nullptr;
        CommandBufferRecorder *postProcessing = nullptr;

    public:
        explicit PassesService(ApplicationContext &context);

        void onInitialize() override;

        void addPass(std::vector<AbstractPass *> &p, AbstractPass *pointer);

        void onSync() override;

        void dispose();
    };
} // Metal

#endif //PASSESSERVICE_H
