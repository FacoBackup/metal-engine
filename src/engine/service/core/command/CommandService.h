#ifndef POOLSERVICE_H
#define POOLSERVICE_H
#include "../../AbstractResourceService.h"
#include "../../../repository/backend/CommandDescriptorRepository.h"

namespace Metal {
    struct CommandBufferInstance;
    struct PipelineInstance;

    class CommandService final : public AbstractResourceService {
        CommandDescriptorRepository &poolRepository;

    public:
        explicit CommandService(ApplicationContext &context);

        void onInitialize() override;

        [[nodiscard]] CommandBufferInstance *createCommandBuffer(PipelineInstance *pipeline) const;

        VkDescriptorSet createDescriptorSet();
    };
}

#endif
