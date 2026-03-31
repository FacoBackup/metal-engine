#ifndef METAL_ENGINE_SCENEENTITYDATA_H
#define METAL_ENGINE_SCENEENTITYDATA_H
#include <optional>
#include "engine/dto/StaticGeometryComponent.h"
#include "engine/dto/TransformComponent.h"
#include "engine/dto/MetadataComponent.h"

#include "common/Reflection.h"

namespace Metal {
    struct SceneEntityData final : Reflection {
        MetadataComponent entity;
        TransformComponent transform;
        std::optional<StaticGeometryComponent> primitive;

    protected:
        void registerFields() override {
            registerCompositeField(&entity).setName("entity");
            registerCompositeField(&transform).setName("transform");

            auto primitiveToJson = [this] {
                if (primitive) return primitive->toJson();
                return nlohmann::json();
            };

            auto primitiveFromJson = [this](const nlohmann::json &j) {
                if (!j.is_null() && !j.empty()) {
                    primitive = StaticGeometryComponent();
                    primitive->fromJson(j);
                } else {
                    primitive = std::nullopt;
                }
            };

            registerGenericField(primitiveToJson, primitiveFromJson).setName("primitive");
        }
    };
}

#endif //METAL_ENGINE_SCENEENTITYDATA_H
