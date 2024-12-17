#ifndef ENTITY_H
#define ENTITY_H
#include <unordered_map>
#include <glm/vec3.hpp>

#include "../../../engine-definitions.h"
#include "../../../enum/ComponentType.h"
#include "../../../../common/inspection/Inspectable.h"

namespace Metal {
    class AbstractComponent;

    class Entity final : public Inspectable {
        EntityID id;

    public:
        const char *onTile = nullptr;
        std::string name = "New entity";
        glm::vec3 color{};
        bool isContainer = false;
        std::unordered_map<ComponentTypes::ComponentType, AbstractComponent *> components{};
        std::vector<EntityID> children{};
        EntityID parent = EMPTY_ENTITY;

        explicit Entity(EntityID id, bool container);

        [[nodiscard]] EntityID getId() const {
            return id;
        }

        void registerFields() override;

        const char *getIcon() override;

        const char *getTitle() override {
            return name.c_str();
        }
    };
} // Metal

#endif //ENTITY_H
