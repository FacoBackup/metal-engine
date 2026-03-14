#ifndef METAL_ENGINE_SCENEENTITYDATA_H
#define METAL_ENGINE_SCENEENTITYDATA_H
#include <optional>
#include "../../engine/dto/PrimitiveComponent.h"
#include "../../engine/dto/TransformComponent.h"
#include "../../engine/dto/MetadataComponent.h"

namespace Metal {

    struct SceneEntityData final : ISerialize {
        MetadataComponent entity;
        TransformComponent transform;
        std::optional<PrimitiveComponent> primitive;

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["entity"] = entity.toJson();
            j["transform"] = transform.toJson();
            if (primitive) j["primitive"] = primitive->toJson();
            return j;
        }

        void fromJson(const nlohmann::json &j) override {
            entity.fromJson(j.at("entity"));
            transform.fromJson(j.at("transform"));
            if (j.contains("primitive")) {
                primitive = PrimitiveComponent();
                primitive->fromJson(j.at("primitive"));
            }
        }
    };
}

#endif //METAL_ENGINE_SCENEENTITYDATA_H