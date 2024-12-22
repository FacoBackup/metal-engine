#include "FilesService.h"
#include "../../common/util/files/FileEntry.h"
#include "../../common/util/files/EntryType.h"
#include "../../engine/engine-definitions.h"
#include "../../common/util/files/FilesUtil.h"
#include "../../context/ApplicationContext.h"
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

#include "../../engine/enum/LevelOfDetail.h"

namespace fs = std::filesystem;

#define DELETE_F(F)\
std::filesystem::remove_all(entry.second->absolutePath);\
std::filesystem::remove_all(F(entry.second->getId(), LevelOfDetail::LOD_0));\
std::filesystem::remove_all(F(entry.second->getId(), LevelOfDetail::LOD_1));\
std::filesystem::remove_all(F(entry.second->getId(), LevelOfDetail::LOD_2));\
std::filesystem::remove_all(F(entry.second->getId(), LevelOfDetail::LOD_3));

#define DELETE_S(F)\
std::filesystem::remove_all(entry.second->absolutePath);\
std::filesystem::remove_all(F(entry.second->getId()));

#define DATA \
        auto fileSizeInBytes = fs::file_size(entry.path());\
        std::string fileSize;\
        if (fileSizeInBytes >= 1'000'000'000) {\
        fileSize = std::to_string(fileSizeInBytes / 1'000'000'000.0) + " GB";\
        } else if (fileSizeInBytes >= 1'000'000) {\
        fileSize = std::to_string(fileSizeInBytes / 1'000'000.0) + " MB";\
        } else if (fileSizeInBytes >= 1'000) {\
        fileSize = std::to_string(fileSizeInBytes / 1'000.0) + " KB";\
        } else {\
        fileSize = std::to_string(fileSizeInBytes) + " Bytes";\
        }\
        std::filesystem::file_time_type ftime = last_write_time(entry);

namespace Metal {
    void FilesService::onInitialize() {
        root = new FileEntry(nullptr, context.getAssetRefDirectory(), "", "");
        root->type = EntryType::DIRECTORY;
        root->name = "Files";
        GetEntries(root);
    }

    std::unique_ptr<FileEntry> FilesService::getResource(const std::string &id) {
        try {
            for (const auto &entry: fs::recursive_directory_iterator(root->absolutePath)) {
                if (entry.is_regular_file() &&
                    entry.path().filename().string() == id + FILE_METADATA) {
                    DATA
                    auto child = std::make_unique<FileEntry>(
                        root,
                        absolute(entry.path()).string(),
                        std::format("{}", ftime),
                        fileSize);
                    std::string json;
                    FilesUtil::ReadFile(child->absolutePath.c_str(), json);
                    child->deserialize(json);
                    return child;
                }
            }
        } catch (const fs::filesystem_error &e) {
            std::cerr << "Error while accessing directory: " << e.what() << '\n';
        }
        return nullptr;
    }

    void FilesService::DeleteFiles(const std::unordered_map<std::string, FileEntry *> &selected) {
        for (auto &entry: selected) {
            switch (entry.second->type) {
                case EntryType::DIRECTORY: {
                    FilesService::GetEntries(entry.second);
                    std::unordered_map<std::string, FileEntry *> files;
                    for (auto *f: entry.second->children) {
                        files.insert({f->getId(), f});
                    }
                    DeleteFiles(files);
                    std::filesystem::remove_all(entry.second->absolutePath);
                    break;
                }
                case EntryType::MESH: {
                    DELETE_F(FORMAT_FILE_MESH)
                    break;
                }
                case EntryType::MATERIAL: {
                    DELETE_S(FORMAT_FILE_MATERIAL)
                    break;
                }
                case EntryType::TEXTURE: {
                    DELETE_F(FORMAT_FILE_TEXTURE)
                    break;
                }
                case EntryType::SCENE: {
                    DELETE_S(FORMAT_FILE_SCENE)
                    break;
                }
                default: break;;
            }
        }
    }

    void FilesService::Move(FileEntry *toMove,
                            FileEntry *targetDir) {
        fs::rename(toMove->absolutePath,
                   targetDir->absolutePath + "/" + toMove->absolutePath.substr(
                       toMove->absolutePath.find_last_of('/') + 1));

        toMove->children.erase(
            std::ranges::remove(toMove->children, toMove).begin(),
            toMove->children.end());
        toMove->parent = targetDir;
        targetDir->children.push_back(toMove);
        GetEntries(targetDir);
        GetEntries(targetDir->parent);
        GetEntries(toMove);
    }


    void FilesService::GetEntries(FileEntry *root) {
        if (root->type != EntryType::DIRECTORY) {
            return;
        }
        root->children.clear();
        for (const auto &entry: fs::directory_iterator(root->absolutePath)) {
            if (!entry.is_directory()) {
                std::string extension = entry.path().extension().string();
                if (extension.find(FILE_METADATA) != std::string::npos) {
                    DATA
                    auto &child = root->children.emplace_back(new FileEntry(
                        root,
                        fs::absolute(entry.path()).string(),
                        std::format("{}", ftime),
                        fileSize));
                    std::string json;
                    FilesUtil::ReadFile(child->absolutePath.c_str(), json);
                    child->deserialize(json);
                }
            } else {
                auto &child = root->children.emplace_back(new FileEntry(
                    root,
                    fs::absolute(entry.path()).string(),
                    "",
                    ""));

                child->name = entry.path().filename();
                child->type = EntryType::DIRECTORY;
            }
        }
    }
} // Metal
