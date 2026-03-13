#ifndef PICKINGSERVICE_H
#define PICKINGSERVICE_H

#include "../../common/AbstractResourceService.h"
#include "../enum/engine-definitions.h"
#include <optional>

namespace Metal {
    struct TextureInstance;
    class PickingService final : public AbstractRuntimeComponent {
    public:
        [[nodiscard]] std::optional<entt::entity> pickEntityFromGBuffer(TextureInstance *attachment, uint32_t pixelX, uint32_t pixelY) const;
    };
} // Metal

#endif // PICKINGSERVICE_H
