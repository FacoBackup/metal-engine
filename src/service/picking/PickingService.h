#ifndef PICKINGSERVICE_H
#define PICKINGSERVICE_H

#include "../abstract/AbstractResourceService.h"
#include "../../enum/engine-definitions.h"
#include <optional>

namespace Metal {
    struct FrameBufferInstance;
    class PickingService final : public AbstractRuntimeComponent {
    public:

        [[nodiscard]] std::optional<EntityID> pickEntityFromGBuffer(const FrameBufferInstance *gBuffer, uint32_t pixelX, uint32_t pixelY) const;
    };
} // Metal

#endif // PICKINGSERVICE_H
