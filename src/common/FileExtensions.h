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
        std::string icon;
    };

    struct FileExtensions {
        inline static const std::unique_ptr<FileExtensionInfo> scene = std::make_unique<FileExtensionInfo>(
            ".scene", "scene", "Scene", Icons::image);
        inline static const std::unique_ptr<FileExtensionInfo> svo = std::make_unique<FileExtensionInfo>(
            ".svo", "svo", "Volume", Icons::view_agenda);
        inline static const std::unique_ptr<FileExtensionInfo> mesh = std::make_unique<FileExtensionInfo>(
            ".mesh", "mesh", "Mesh", Icons::view_in_ar);

        inline static const std::unique_ptr<FileExtensionInfo> png = std::make_unique<FileExtensionInfo>(
            ".png", "png", "Texture", Icons::texture);
        inline static const std::unique_ptr<FileExtensionInfo> jpg = std::make_unique<FileExtensionInfo>(
            ".jpg", "jpg", "Texture", Icons::texture);
        inline static const std::unique_ptr<FileExtensionInfo> jpeg = std::make_unique<FileExtensionInfo>(
            ".jpeg", "jpeg", "Texture", Icons::texture);
        inline static const std::unique_ptr<FileExtensionInfo> tga = std::make_unique<FileExtensionInfo>(
            ".tga", "tga", "Texture", Icons::texture);

        inline static const std::unique_ptr<FileExtensionInfo> vdb = std::make_unique<FileExtensionInfo>(
            ".vdb", "vdb", "OpenVDB", Icons::view_agenda);

        inline static const std::unique_ptr<FileExtensionInfo> glb = std::make_unique<FileExtensionInfo>(
            ".glb", "glb", "Scene Source", Icons::image);
        inline static const std::unique_ptr<FileExtensionInfo> gltf = std::make_unique<FileExtensionInfo>(
            ".gltf", "gltf", "Scene Source", Icons::image);
        inline static const std::unique_ptr<FileExtensionInfo> fbx = std::make_unique<FileExtensionInfo>(
            ".fbx", "fbx", "Scene Source", Icons::image);
        inline static const std::unique_ptr<FileExtensionInfo> obj = std::make_unique<FileExtensionInfo>(
            ".obj", "obj", "Scene Source", Icons::image);

        inline static const std::unique_ptr<FileExtensionInfo> lua = std::make_unique<FileExtensionInfo>(
            ".lua", "lua", "Script", Icons::insert_drive_file);

        inline static const std::vector<const FileExtensionInfo *> all = {
            scene.get(), svo.get(), mesh.get(), png.get(), jpg.get(), jpeg.get(), tga.get(), vdb.get(), glb.get(),
            gltf.get(), fbx.get(), obj.get(), lua.get()
        };

        inline static const std::vector<const FileExtensionInfo *> scripts = {
            lua.get()
        };
    };
}

#endif //METAL_ENGINE_FILEEXTENSIONS_H
