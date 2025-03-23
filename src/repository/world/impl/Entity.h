#ifndef ENTITY_H
#define ENTITY_H
#include <unordered_map>
#include <glm/vec3.hpp>

#include "../../../enum/engine-definitions.h"
#include "../../../enum/ComponentType.h"
#include "../../../common/inspection/Inspectable.h"

#include "../../../util/serialization-definitions.h"

namespace Metal {
    struct AbstractComponent;

    class Entity final : public Inspectable {
        EntityID id;

    public:
        std::string name = "New entity";
        glm::vec3 color{};
        bool isContainer = false;
        std::vector<ComponentTypes::ComponentType> components{};
        std::vector<EntityID> children{};
        EntityID parent = EMPTY_ENTITY;

        SERIALIZE_TEMPLATE(id, parent, name, color.x, color.y, color.z, isContainer, components, children)

        Entity() = default;

        void initialize(EntityID id, bool container);

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
