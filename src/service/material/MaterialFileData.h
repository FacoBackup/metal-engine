#ifndef MATERIAL_FILE_DATA_H
#define MATERIAL_FILE_DATA_H
#include <string>
#include <glm/vec3.hpp>

#include "../../common/inspection/Inspectable.h"
#include "../../util/serialization-definitions.h"

namespace Metal {
    struct MaterialFileData final : Inspectable {
        std::string albedo;
        std::string normal;
        std::string roughness;
        std::string metallic;
        std::string height;

        void registerFields() override;

        const char *getIcon() override;

        const char *getTitle() override;

        SAVE_TEMPLATE(
            albedo,
            normal,
            roughness,
            metallic,
            height)
    };
}

#endif //MATERIALDATA_H
