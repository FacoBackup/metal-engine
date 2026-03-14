#ifndef PICKINGSERVICE_H
#define PICKINGSERVICE_H

#include "../../common/IService.h"
#include "../enum/engine-definitions.h"
#include <optional>

namespace Metal {
    struct TextureInstance;
    class VulkanContext;
    class BufferService;
    struct WorldRepository;

    class PickingService final : public IService {
        VulkanContext *vulkanContext = nullptr;
        BufferService *bufferService = nullptr;
        WorldRepository *worldRepository = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"VulkanContext", vulkanContext},
                {"BufferService", bufferService},
                {"WorldRepository", worldRepository}
            };
        }

        [[nodiscard]] std::optional<entt::entity> pickEntityFromGBuffer(TextureInstance *attachment, uint32_t pixelX, uint32_t pixelY) const;
    };
} // Metal

#endif // PICKINGSERVICE_H
