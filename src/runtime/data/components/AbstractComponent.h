#ifndef ABSTRACTCOMPONENT_H
#define ABSTRACTCOMPONENT_H
#include "../../../common/inspection/Inspectable.h"
#include "../../../common/engine-definitions.h"
#include "../../../common/enum/ComponentType.h"

namespace Metal {
    struct AbstractComponent : Inspectable {
        AbstractComponent() = default;

        [[nodiscard]] EntityID getEntityId() const {
            return entityId;
        }

        void setEntityId(const EntityID entityId) {
            this->entityId = entityId;
        }

        const char *getIcon() override {
            return ComponentTypes::IconOf(getType());
        }

        const char *getTitle() override {
            return ComponentTypes::NameOf(getType());
        }

        virtual ComponentTypes::ComponentType getType() {
            throw std::runtime_error("Not implemented");
        }

    protected:
        EntityID entityId{};
    };
} // Metal

#endif //ABSTRACTCOMPONENT_H
