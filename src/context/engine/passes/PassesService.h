#ifndef PASSESSERVICE_H
#define PASSESSERVICE_H
#include <memory>
#include <vector>

#include "AbstractPass.h"
#include "../../../common/AbstractRuntimeComponent.h"


namespace Metal {
    class CommandBufferRecorder;

    class PassesService final : public AbstractRuntimeComponent {
        std::vector<std::unique_ptr<AbstractPass> > computePasses;
        std::vector<std::unique_ptr<AbstractPass> > giPasses;
        std::vector<std::unique_ptr<AbstractPass> > fullScreenRenderPasses;
        std::vector<std::unique_ptr<AbstractPass> > gBufferPasses;
        std::vector<std::unique_ptr<AbstractPass> > postProcessingPasses;

        CommandBufferRecorder *gBuffer = nullptr;
        CommandBufferRecorder *fullScreen = nullptr;
        CommandBufferRecorder *postProcessing = nullptr;
        CommandBufferRecorder *compute = nullptr;
    public:
        explicit PassesService(ApplicationContext &context);

        void onInitialize() override;

        void onSync() override;
    };
} // Metal

#endif //PASSESSERVICE_H
