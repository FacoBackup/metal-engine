#ifndef WORLDREPOSITORY_H
#define WORLDREPOSITORY_H

#include <unordered_map>

#include "../../engine-definitions.h"
#include "../../../common/interface/Initializable.h"
#include "impl/Entity.h"
#include "../../enum/ComponentType.h"

namespace Metal {
    struct Camera;

    struct WorldRepository final {
        static constexpr EntityID ROOT_ID = 1;

        explicit WorldRepository();

        Camera *camera;
        std::unordered_map<EntityID, std::unique_ptr<Entity> > entities{};
        std::unordered_map<EntityID, bool> culled{};
        std::unordered_map<EntityID, bool> hiddenEntities{};
        void createComponent(EntityID entity, ComponentTypes::ComponentType type) const;

        EntityID createEntity(std::string name = "New Entity", bool container = false);

        void linkEntities(const Entity *target, Entity *toLink) const;

        Entity *getEntity(EntityID node) const;

    private:
        EntityID lastId = ROOT_ID;
    };
} // Metal

#endif //WORLDREPOSITORY_H
