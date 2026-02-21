#ifndef MATERIAL_FILE_DATA_H
#define MATERIAL_FILE_DATA_H
#include <string>

#include "../../common/inspection/Inspectable.h"
#include "../../util/Serializable.h"

namespace Metal {
    struct MaterialFileData final : Inspectable, Serializable {
        std::string albedo;
        std::string normal;
        std::string roughness;
        std::string metallic;
        std::string height;

        void registerFields() override;

        const char *getIcon() override;

        const char *getTitle() override;

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["albedo"] = albedo;
            j["normal"] = normal;
            j["roughness"] = roughness;
            j["metallic"] = metallic;
            j["height"] = height;
            return j;
        }

        void fromJson(const nlohmann::json& j) override {
            albedo = j.at("albedo").get<std::string>();
            normal = j.at("normal").get<std::string>();
            roughness = j.at("roughness").get<std::string>();
            metallic = j.at("metallic").get<std::string>();
            height = j.at("height").get<std::string>();
        }
    };
}

#endif //MATERIALDATA_H
