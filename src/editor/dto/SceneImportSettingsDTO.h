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
            registerEditableField<BOOLEAN>(&triangulate).setName("Triangulate").setLabel("Mesh");
            registerEditableField<BOOLEAN>(&flipUVs).setName("Flip UVs").setLabel("Texture");
            registerEditableField<BOOLEAN>(&genSmoothNormals).setName("Gen Smooth Normals").setLabel("Mesh");
            registerEditableField<BOOLEAN>(&generateUVs).setName("Generate UVs").setLabel("Mesh");
            registerEditableField<FLOAT>(&scale).setName("Global Scale").setLabel("Transform").setMin(0.001f).setMax(1000.0f);
        }

        const char *getTitle() const override { return "Scene Import Settings"; }
    };
}

#endif
