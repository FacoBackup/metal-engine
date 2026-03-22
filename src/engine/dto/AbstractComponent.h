#ifndef ABSTRACTCOMPONENT_H
#define ABSTRACTCOMPONENT_H
#include "../../common/Inspectable.h"
#include "../enum/ComponentType.h"

namespace Metal {
    struct AbstractComponent : Inspectable {
        AbstractComponent() = default;

        [[nodiscard]] entt::entity getEntityId() const {
            return entityId;
        }

        void setEntityId(const entt::entity entityId) {
            this->entityId = entityId;
        }

        const char *getIcon() const override {
            return Metal::ComponentTypes::IconOf(getType());
        }

        const char *getTitle() const override {
            return Metal::ComponentTypes::NameOf(getType());
        }

        virtual ComponentType getType() const {
            throw std::runtime_error("Not implemented");
        }

    protected:
        entt::entity entityId{};
    };
} // Metal

#endif //ABSTRACTCOMPONENT_H
