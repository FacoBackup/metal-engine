#ifndef POOLSERVICE_H
#define POOLSERVICE_H

#include "../../common/interface/AbstractResourceService.h"

namespace Metal {
    struct CommandBufferInstance;
    struct PipelineInstance;

    class CommandService final : public AbstractResourceService {
    public:
        explicit CommandService(ApplicationContext &context);

        [[nodiscard]] CommandBufferInstance *createCommandBuffer(PipelineInstance *pipeline) const;
    };
}

#endif
