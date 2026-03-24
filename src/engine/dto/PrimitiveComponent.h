#ifndef PRIMITIVE_COMPONENT_H
#define PRIMITIVE_COMPONENT_H

#include "AbstractComponent.h"
#include "../../common/ISerialize.h"
#include <glm/glm.hpp>

namespace Metal {
    struct PrimitiveComponent final : AbstractComponent, ISerialize {
        std::string meshId;

        std::string albedo;
        std::string roughness;
        std::string metallic;
        float transmissionFactor = 0;
        float thicknessFactor = 0;
        float ior = 1.45;
        unsigned int renderIndex = 0;

        void registerFields() override;

        ComponentType getType() const override;

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json& j) override;
    };
}
#endif //MESHCOMPONENT_H
