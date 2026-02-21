#ifndef PLANELIGHTCOMPONENT_H
#define PLANELIGHTCOMPONENT_H

#include "LightComponent.h"

#include <cereal/types/polymorphic.hpp>

namespace Metal {
    struct PlaneLightComponent final : LightComponent {
        LightTypes::LightType getLightType() override { return LightTypes::PLANE; }

        SERIALIZE_TEMPLATE(entityId, color.x, color.y, color.z, intensity)
    };
} // Metal

CEREAL_REGISTER_TYPE(Metal::PlaneLightComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Metal::LightComponent, Metal::PlaneLightComponent)

#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>

#endif //PLANELIGHTCOMPONENT_H
