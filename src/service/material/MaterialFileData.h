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
        glm::vec3 albedoColor{1, 1, 1};
        float roughnessFactor = 1;
        float metallicFactor = 0;
        float transmissionFactor = 0;
        float thicknessFactor = 0;
        float ior = 1.45;
        bool isEmissive = false;

        void registerFields() override;

        const char *getIcon() override;

        const char *getTitle() override;

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json &j) override;
    };
}

#endif //MATERIALDATA_H
