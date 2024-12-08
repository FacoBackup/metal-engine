#ifndef POOLSERVICE_H
#define POOLSERVICE_H
#include "../../AbstractResourceService.h"
#include "../../../repository/backend/CommandPoolRepository.h"

namespace Metal {
    class CommandBufferInstance;

    class CommandPoolService final : public AbstractResourceService {
        CommandPoolRepository poolRepository;

    public:
        explicit CommandPoolService(ApplicationContext &context);

        void onInitialize() override;

        [[nodiscard]] CommandBufferInstance *createCommandBuffer() const;

        VkDescriptorSet createDescriptorSet();
    };
}

#endif
