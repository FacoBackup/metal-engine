#ifndef MATERIAL_FILE_DATA_H
#define MATERIAL_FILE_DATA_H
#include <string>

#include "../../common/inspection/Inspectable.h"
#include "../../util/Serializable.h"
#include <glm/glm.hpp>

namespace Metal {
    struct MaterialFileData final : Inspectable, Serializable {
        std::string albedo;
        std::string normal;
        std::string roughness;
        std::string metallic;
        std::string ao;
        std::string height;
        glm::vec3 albedoColor;
        float roughnessFactor;
        float metallicFactor;
        bool isEmissive;

        void registerFields() override;

        const char *getIcon() override;

        const char *getTitle() override;

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json &j) override;
    };
}

#endif //MATERIALDATA_H
