#ifndef METAL_ENGINE_LIGHTCOMPONENT_H
#define METAL_ENGINE_LIGHTCOMPONENT_H
#include "AbstractComponent.h"
#include "common/ISerialize.h"

namespace Metal {
    struct LightComponent final : AbstractComponent, ISerialize {

        ComponentType getType() const override;

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json& j) override;
    };
} // Metal

#endif //METAL_ENGINE_LIGHTCOMPONENT_H
