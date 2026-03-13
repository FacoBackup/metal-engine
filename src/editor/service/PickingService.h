#ifndef PICKINGSERVICE_H
#define PICKINGSERVICE_H

#include "../../common/AbstractResourceService.h"
#include "../enum/engine-definitions.h"
#include <optional>

namespace Metal {
    struct TextureInstance;
    class VulkanContext;
    class BufferService;
    struct WorldRepository;

    class PickingService final : public AbstractRuntimeComponent {
        VulkanContext &vulkanContext;
        BufferService &bufferService;
        WorldRepository &worldRepository;

    public:
        PickingService(VulkanContext &vulkanContext, BufferService &bufferService, WorldRepository &worldRepository)
            : vulkanContext(vulkanContext), bufferService(bufferService), worldRepository(worldRepository) {}
        PickingService() = delete;

        [[nodiscard]] std::optional<entt::entity> pickEntityFromGBuffer(TextureInstance *attachment, uint32_t pixelX, uint32_t pixelY) const;
    };
} // Metal

#endif // PICKINGSERVICE_H
