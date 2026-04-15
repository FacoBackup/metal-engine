#ifndef METAL_ENGINE_FILEEXTENSIONS_H
#define METAL_ENGINE_FILEEXTENSIONS_H

#include <string>
#include <vector>
#include <memory>
#include "Icons.h"

namespace Metal {
    struct FileExtensionInfo {
        std::string extension; // with "."
        std::string name; // without "."
        std::string typeLabel;
        const std::string &icon;
    };

    struct FileExtensions {
        inline static const std::unique_ptr<FileExtensionInfo> scene = std::make_unique<FileExtensionInfo>(
            ".mscene", "scene", "Scene", Icons::inventory_2);
        inline static const std::unique_ptr<FileExtensionInfo> geometry = std::make_unique<FileExtensionInfo>(
            ".mgeometry", "geometry", "Geometry", Icons::view_in_ar);
        inline static const std::unique_ptr<FileExtensionInfo> level = std::make_unique<FileExtensionInfo>(
            ".mlevel", "level", "Level", Icons::forest);

        inline static const std::unique_ptr<FileExtensionInfo> png = std::make_unique<FileExtensionInfo>(
            ".png", "png", "Texture", Icons::texture);
        inline static const std::unique_ptr<FileExtensionInfo> jpg = std::make_unique<FileExtensionInfo>(
            ".jpg", "jpg", "Texture", Icons::texture);
        inline static const std::unique_ptr<FileExtensionInfo> jpeg = std::make_unique<FileExtensionInfo>(
            ".jpeg", "jpeg", "Texture", Icons::texture);
        inline static const std::unique_ptr<FileExtensionInfo> tga = std::make_unique<FileExtensionInfo>(
            ".tga", "tga", "Texture", Icons::texture);

        inline static const std::unique_ptr<FileExtensionInfo> glb = std::make_unique<FileExtensionInfo>(
            ".glb", "glb", "Scene Source", Icons::category);
        inline static const std::unique_ptr<FileExtensionInfo> gltf = std::make_unique<FileExtensionInfo>(
            ".gltf", "gltf", "Scene Source", Icons::category);
        inline static const std::unique_ptr<FileExtensionInfo> fbx = std::make_unique<FileExtensionInfo>(
            ".fbx", "fbx", "Scene Source", Icons::category);
        inline static const std::unique_ptr<FileExtensionInfo> obj = std::make_unique<FileExtensionInfo>(
            ".obj", "obj", "Scene Source", Icons::category);

        inline static const std::unique_ptr<FileExtensionInfo> lua = std::make_unique<FileExtensionInfo>(
            ".lua", "lua", "Lua Script", Icons::script);
        inline static const std::unique_ptr<FileExtensionInfo> material = std::make_unique<FileExtensionInfo>(
            ".mmat", "mmat", "Material", Icons::palette);


        inline static const std::vector<const FileExtensionInfo *> all = {
            scene.get(), geometry.get(), level.get(), png.get(), jpg.get(), jpeg.get(), tga.get(), glb.get(),
            gltf.get(), fbx.get(), obj.get(), lua.get(), level.get(), material.get()
        };

        inline static const std::vector<const FileExtensionInfo *> scripts = {
            lua.get()
        };
    };
}

#endif //METAL_ENGINE_FILEEXTENSIONS_H
