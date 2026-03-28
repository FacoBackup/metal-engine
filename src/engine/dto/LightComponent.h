#ifndef METAL_ENGINE_LIGHTCOMPONENT_H
#define METAL_ENGINE_LIGHTCOMPONENT_H
#include "AbstractComponent.h"
#include <glm/vec3.hpp>

namespace Metal {
    struct LightComponent final : AbstractComponent {
        glm::vec3 color = glm::vec3(1.0f);

    protected:
        void registerFields() override;

    public:
        ComponentType getType() const override;
    };
} // Metal

#endif //METAL_ENGINE_LIGHTCOMPONENT_H
