#ifndef ENTITY_H
#define ENTITY_H
#include <unordered_map>
#include <glm/vec3.hpp>

#include "AbstractComponent.h"
#include "editor/enum/engine-definitions.h"
#include "../enum/ComponentType.h"
#include "../../common/Reflection.h"


namespace Metal {
    struct MetadataComponent final : AbstractComponent {
        std::string name = "New entity";

    protected:
        void registerFields() override;

    public:
        ComponentType getType() const override;
    };
} // Metal

#endif //ENTITY_H
