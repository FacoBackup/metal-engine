#ifndef MATERIAL_H
#define MATERIAL_H

#include "../../common/Reflection.h"
#include "../../common/FileExtensions.h"
#include <string>

namespace Metal {
    struct Material final : public Reflection {
        std::string albedo = "";
        std::string normal = "";
        std::string roughness = "";
        std::string metallic = "";
        std::string emissive = "";
        float roughnessFactor = 1.0f;
        float metallicFactor = 0.0f;
        float parallaxScale = 0.05f;
        int parallaxLayers = 32;

        void registerFields() override {
            static const std::vector<const FileExtensionInfo *> textureExtensions = {
                FileExtensions::png.get(), FileExtensions::jpg.get(), FileExtensions::jpeg.get(), FileExtensions::tga.get()
            };
            registerEditableField<RESOURCE>(&albedo).setName("albedo").setLabel("Albedo").setSupportedFileTypes(textureExtensions);
            registerEditableField<RESOURCE>(&normal).setName("normal").setLabel("Normal").setSupportedFileTypes(textureExtensions);
            registerEditableField<RESOURCE>(&roughness).setName("roughness").setLabel("Roughness").setSupportedFileTypes(textureExtensions);
            registerEditableField<RESOURCE>(&metallic).setName("metallic").setLabel("Metallic").setSupportedFileTypes(textureExtensions);
            registerEditableField<RESOURCE>(&emissive).setName("emissive").setLabel("Emissive").setSupportedFileTypes(textureExtensions);
            registerEditableField<FLOAT>(&roughnessFactor).setName("roughnessFactor").setLabel("Roughness Factor").setMin(0.0f).setMax(1.0f).setIncrement(0.01f);
            registerEditableField<FLOAT>(&metallicFactor).setName("metallicFactor").setLabel("Metallic Factor").setMin(0.0f).setMax(1.0f).setIncrement(0.01f);
            registerEditableField<FLOAT>(&parallaxScale).setName("parallaxScale").setLabel("Parallax Scale").setMin(0.0f).setMax(1.0f).setIncrement(0.005f);
            registerEditableField<INT>(&parallaxLayers).setName("parallaxLayers").setLabel("Parallax Layers").setMin(1).setMax(128).setIncrement(1);
        }

        const char *getIcon() const override {
            return "palette";
        }
    };
} // Metal

#endif // MATERIAL_H
