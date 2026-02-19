#ifndef PICKINGSERVICE_H
#define PICKINGSERVICE_H

#include "../abstract/AbstractResourceService.h"
#include "../../enum/engine-definitions.h"
#include <optional>

namespace Metal {
    class PickingService final : public AbstractResourceService {
    public:
        explicit PickingService()
            : AbstractResourceService() {
        }

        [[nodiscard]] std::optional<EntityID> pickEntityFromGBuffer(uint32_t pixelX, uint32_t pixelY) const;
    };
} // Metal

#endif // PICKINGSERVICE_H
