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
            registerBool(triangulate, "Mesh", "Triangulate");
            registerBool(flipUVs, "Texture", "Flip UVs");
            registerBool(genSmoothNormals, "Mesh", "Gen Smooth Normals");
            registerBool(generateUVs, "Mesh", "Generate UVs");
            registerFloat(scale, "Transform", "Global Scale", 0.001f, 1000.0f);
        }

        const char *getTitle() override { return "Scene Import Settings"; }
    };
}

#endif
