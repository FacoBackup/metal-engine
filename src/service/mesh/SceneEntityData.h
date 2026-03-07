#ifndef METAL_ENGINE_SCENEENTITYDATA_H
#define METAL_ENGINE_SCENEENTITYDATA_H
#include <optional>
#include "../../repository/world/components/PrimitiveComponent.h"
#include "../../repository/world/components/SphereLightComponent.h"
#include "../../repository/world/components/PlaneLightComponent.h"
#include "../../repository/world/components/TransformComponent.h"
#include "../../repository/world/impl/MetadataComponent.h"

namespace Metal {

    struct SceneEntityData final : Serializable {
        MetadataComponent entity;
        TransformComponent transform;
        std::optional<PrimitiveComponent> primitive;
        std::optional<SphereLightComponent> sphereLight;
        std::optional<PlaneLightComponent> planeLight;

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["entity"] = entity.toJson();
            j["transform"] = transform.toJson();
            if (primitive) j["primitive"] = primitive->toJson();
            if (sphereLight) j["sphereLight"] = sphereLight->toJson();
            if (planeLight) j["planeLight"] = planeLight->toJson();
            return j;
        }

        void fromJson(const nlohmann::json &j) override {
            entity.fromJson(j.at("entity"));
            transform.fromJson(j.at("transform"));
            if (j.contains("primitive")) {
                primitive = PrimitiveComponent();
                primitive->fromJson(j.at("primitive"));
            }
            if (j.contains("sphereLight")) {
                sphereLight = SphereLightComponent();
                sphereLight->fromJson(j.at("sphereLight"));
            }
            if (j.contains("planeLight")) {
                planeLight = PlaneLightComponent();
                planeLight->fromJson(j.at("planeLight"));
            }
        }
    };
}

#endif //METAL_ENGINE_SCENEENTITYDATA_H