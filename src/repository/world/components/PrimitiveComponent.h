#ifndef PRIMITIVE_COMPONENT_H
#define PRIMITIVE_COMPONENT_H

#include "../impl/AbstractComponent.h"
#include "../../../util/Serializable.h"
#include <glm/glm.hpp>

namespace Metal {
    struct PrimitiveComponent final : AbstractComponent, Serializable {
        std::string meshId;

        std::string albedo;
        std::string roughness;
        std::string metallic;
        glm::vec3 albedoColor{1, 1, 1};
        float roughnessFactor = 1;
        float metallicFactor = 0;
        float transmissionFactor = 0;
        float thicknessFactor = 0;
        float ior = 1.45;
        bool isEmissive = false;

        unsigned int renderIndex = 0;

        void registerFields() override;

        void onUpdate(InspectableMember *member) override;

        ComponentType getType() override;

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json& j) override;
    };
}
#endif //MESHCOMPONENT_H
