#ifndef SCENESETTINGSDTO_H
#define SCENESETTINGSDTO_H

#include "ImportSettingsDTO.h"

namespace Metal {
    struct SceneImportSettingsDTO final : ImportSettingsDTO {
        bool triangulate = true;
        bool flipUVs = true;
        bool genSmoothNormals = true;
        bool generateUVs = true;
        float scale = 1.0f;

        void registerFields() override {
            registerEditableField(&triangulate, BOOLEAN, "Triangulate", "Mesh");
            registerEditableField(&flipUVs, BOOLEAN, "Flip UVs", "Texture");
            registerEditableField(&genSmoothNormals, BOOLEAN, "Gen Smooth Normals", "Mesh");
            registerEditableField(&generateUVs, BOOLEAN, "Generate UVs", "Mesh");
            registerEditableField(&scale, FLOAT, "Global Scale", "Transform").setMin(0.001f).setMax(1000.0f);
        }

        const char *getTitle() const override { return "Scene Import Settings"; }
    };
}

#endif
