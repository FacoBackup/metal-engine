#ifndef ABSTRACTCOMPONENT_H
#define ABSTRACTCOMPONENT_H
#include "../../../common/inspection/Inspectable.h"
#include "../../../enum/engine-definitions.h"
#include "../../../enum/ComponentType.h"

namespace Metal {
    struct AbstractComponent : Inspectable {
        AbstractComponent() = default;

        EntityID getEntityId() const {
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

    private:
        EntityID entityId{};
    };
} // Metal

#endif //ABSTRACTCOMPONENT_H
