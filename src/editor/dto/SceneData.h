#ifndef SCENEDESCRIPTION_H
#define SCENEDESCRIPTION_H
#include <vector>

#include "SceneEntityData.h"
#include "../../common/Reflection.h"

namespace Metal {

    struct SceneData final : Reflection {
        std::vector<SceneEntityData> entities;
        std::string name;

        void registerFields() override {
            registerSerializableOnlyField<STRING>(&name).setName("name");
            registerGenericField([this] {
                nlohmann::json j = nlohmann::json::array();
                for (auto &entity: entities) j.push_back(entity.toJson());
                return j;
            }, [this](const nlohmann::json &j) {
                entities.clear();
                for (const auto &item: j) {
                    SceneEntityData entity;
                    entity.fromJson(item);
                    entities.push_back(entity);
                }
            }).setName("entities");
        }
    };
}

#endif //SCENEDESCRIPTION_H
