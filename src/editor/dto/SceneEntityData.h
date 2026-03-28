#ifndef METAL_ENGINE_SCENEENTITYDATA_H
#define METAL_ENGINE_SCENEENTITYDATA_H
#include <optional>
#include "engine/dto/PrimitiveComponent.h"
#include "engine/dto/TransformComponent.h"
#include "engine/dto/MetadataComponent.h"

#include "common/Reflection.h"

namespace Metal {
    struct SceneEntityData final : Reflection {
        MetadataComponent entity;
        TransformComponent transform;
        std::optional<PrimitiveComponent> primitive;

    protected:
        void registerFields() override {
            registerSerializableOnlyField(&entity, COMPOSITE, "entity");
            registerSerializableOnlyField(&transform, COMPOSITE, "transform");

            auto primitiveToJson = [this] {
                if (primitive) return primitive->toJson();
                return nlohmann::json();
            };

            auto primitiveFromJson = [this](const nlohmann::json &j) {
                if (!j.is_null()) {
                    primitive = PrimitiveComponent();
                    primitive->fromJson(j);
                } else {
                    primitive = std::nullopt;
                }
            };

            registerSerializableOnlyField(nullptr, COMPOSITE, "primitive")
                    .setTransformer(primitiveToJson, primitiveFromJson);
        }
    };
}

#endif //METAL_ENGINE_SCENEENTITYDATA_H
