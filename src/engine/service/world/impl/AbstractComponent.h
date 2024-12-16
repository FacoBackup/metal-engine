#ifndef ABSTRACTCOMPONENT_H
#define ABSTRACTCOMPONENT_H
#include "../../../../common/inspection/Inspectable.h"
#include "../../../engine-definitions.h"

namespace Metal {
    struct AbstractComponent : Inspectable {
        explicit AbstractComponent(const EntityID entityId) : entityId(entityId) {
        }

        EntityID getEntityId() const {
            return entityId;
        }

    private:
        EntityID entityId;
    };
} // Metal

#endif //ABSTRACTCOMPONENT_H
