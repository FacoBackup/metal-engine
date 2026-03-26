#ifndef METAL_ENGINE_LIGHTCOMPONENT_H
#define METAL_ENGINE_LIGHTCOMPONENT_H
#include "AbstractComponent.h"
#include "common/ISerialize.h"
#include <glm/vec3.hpp>

namespace Metal {
    struct LightComponent final : AbstractComponent, ISerialize {
        glm::vec3 color = glm::vec3(1.0f);

        void registerFields() override;

        ComponentType getType() const override;

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json& j) override;
    };
} // Metal

#endif //METAL_ENGINE_LIGHTCOMPONENT_H
