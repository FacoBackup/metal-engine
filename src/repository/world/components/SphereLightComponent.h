#ifndef SPHERELIGHTCOMPONENT_H
#define SPHERELIGHTCOMPONENT_H

#include "LightComponent.h"

#include <cereal/types/polymorphic.hpp>

namespace Metal {
    struct SphereLightComponent final : LightComponent {
        float radiusSize = 1;

        LightTypes::LightType getLightType() override { return LightTypes::SPHERE; }

        void registerFields() override;

        SERIALIZE_TEMPLATE(entityId, color.x, color.y, color.z, intensity, radiusSize)
    };
} // Metal

CEREAL_REGISTER_TYPE(Metal::SphereLightComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Metal::LightComponent, Metal::SphereLightComponent)

#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>

#endif //SPHERELIGHTCOMPONENT_H
