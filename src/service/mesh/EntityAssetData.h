#ifndef ENTITYASSETDATA_H
#define ENTITYASSETDATA_H
#include <string>

#include "../../util/Serializable.h"

namespace Metal {
    struct EntityAssetData final : Serializable {
        std::string name{};
        std::string meshId{};
        std::string materialId{};
        int parentEntity = -1;
        int id;

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["id"] = id;
            j["name"] = name;
            j["meshId"] = meshId;
            j["materialId"] = materialId;
            j["parentEntity"] = parentEntity;
            return j;
        }

        void fromJson(const nlohmann::json& j) override {
            id = j.at("id").get<int>();
            name = j.at("name").get<std::string>();
            meshId = j.at("meshId").get<std::string>();
            materialId = j.at("materialId").get<std::string>();
            parentEntity = j.at("parentEntity").get<int>();
        }
    };
}
#endif //ENTITYASSETDATA_H
