#ifndef SCENEDESCRIPTION_H
#define SCENEDESCRIPTION_H
#include <vector>
#include "../../util/Serializable.h"
#include "EntityAssetData.h"

namespace Metal {
    struct SceneData final : Serializable {
        std::vector<EntityAssetData> entities;
        std::string name;

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["name"] = name;
            nlohmann::json e = nlohmann::json::array();
            for (const auto& ent : entities) {
                e.push_back(ent.toJson());
            }
            j["entities"] = e;
            return j;
        }

        void fromJson(const nlohmann::json& j) override {
            name = j.at("name").get<std::string>();
            entities.clear();
            for (const auto& ent : j.at("entities")) {
                EntityAssetData ead;
                ead.fromJson(ent);
                entities.push_back(ead);
            }
        }
    };
}

#endif //SCENEDESCRIPTION_H
