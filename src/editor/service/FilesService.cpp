#include "FilesService.h"
#include "NotificationService.h"
#include "../dto/FSEntry.h"
#include "../enum/EntryType.h"
#include "../enum/engine-definitions.h"
#include "../../common/FilesUtil.h"
#include "../../common/LoggerUtil.h"
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include "../../common/serialization-definitions.h"
#include "../../core/DirectoryService.h"

namespace fs = std::filesystem;

#define DELETE_F(F, rootDir)\
std::filesystem::remove_all(entry.second->absolutePath);\
std::filesystem::remove_all(rootDir + "/assets/" + F(entry.second->getId()));

#define DELETE_S(F, rootDir)\
std::filesystem::remove_all(entry.second->absolutePath);\
std::filesystem::remove_all(rootDir + "/assets/" + F(entry.second->getId()));

#define DATA \
        std::filesystem::file_time_type ftime = last_write_time(entry);

namespace Metal {
    void FilesService::onInitialize() {
        root = new FSEntry(nullptr, directoryService->getAssetRefDirectory(), "");
        root->type = EntryType::DIRECTORY;
        root->name = "Files";
        GetEntries(root);
    }

    std::unique_ptr<FSEntry> FilesService::getResource(const std::string &id) {
        try {
            for (const auto &entry: fs::recursive_directory_iterator(root->absolutePath)) {
                if (entry.is_regular_file() &&
                    entry.path().filename().string() == id + FILE_METADATA) {
                    DATA
                    auto sys_tp = std::chrono::file_clock::to_utc(ftime);
                    std::string dateStr = std::format("{:%Y-%m-%d %H:%M}", sys_tp);
                    auto child = std::make_unique<FSEntry>(
                        root,
                        absolute(entry.path()).string(),
                        dateStr);
                    PARSE_TEMPLATE(*child, child->absolutePath.c_str())
                    child->formattedSize = FilesUtil::FormatSize(child->size);
                    return child;
                }
            }
        } catch (const fs::filesystem_error &e) {
            std::cerr << "Error while accessing directory: " << e.what() << '\n';
        }
        return nullptr;
    }

    void FilesService::deleteFiles(const std::unordered_map<std::string, FSEntry *> &selected) {
        for (auto &entry: selected) {
            switch (entry.second->type) {
                case EntryType::DIRECTORY: {
                    GetEntries(entry.second);
                    std::unordered_map<std::string, FSEntry *> files;
                    for (auto *f: entry.second->children) {
                        files.insert({f->getId(), f});
                    }
                    deleteFiles(files);
                    std::filesystem::remove_all(entry.second->absolutePath);
                    break;
                }
                case EntryType::MESH: {
                    DELETE_F(FORMAT_FILE_MESH, directoryService->getRootDirectory())
                    break;
                }
                case EntryType::TEXTURE: {
                    DELETE_F(FORMAT_FILE_TEXTURE, directoryService->getRootDirectory())
                    break;
                }
                case EntryType::SCENE: {
                    DELETE_S(FORMAT_FILE_SCENE, directoryService->getRootDirectory())
                    break;
                }
                case EntryType::VOLUME: {
                    DELETE_S(FORMAT_FILE_VOLUME, directoryService->getRootDirectory())
                    break;
                }
                default: break;;
            }
        }
    }

    void FilesService::CreateDirectory(FSEntry *currentDirectory) {
        int count = 0;
        for (FSEntry *child: currentDirectory->children) {
            if (child->type == EntryType::DIRECTORY && child->name == "New Directory (" + std::to_string(count) + ")") {
                count++;
            }
        }
        std::string pathToDir = currentDirectory->absolutePath + '/' + "New Directory (" + std::to_string(count + 1) + ")";
        if (std::filesystem::exists(pathToDir)) {
            return;
        }
        std::filesystem::create_directory(pathToDir);
    }

    void FilesService::Move(FSEntry *toMove, FSEntry *targetDir) {
        if (!targetDir || targetDir->type != EntryType::DIRECTORY) {
            return;
        }

        fs::path sourcePath = toMove->absolutePath;
        fs::path targetPath = fs::path(targetDir->absolutePath) / sourcePath.filename();

        try {
            fs::rename(sourcePath, targetPath);
        } catch (const fs::filesystem_error &e) {
            LOG_ERROR("Could not move file");
            notificationService->pushMessage("Could not move entry", NotificationSeverities::ERROR);
            return;
        }

        toMove->absolutePath = targetPath.string();

        if (toMove->parent) {
            auto &oldChildren = toMove->parent->children;
            oldChildren.erase(std::remove(oldChildren.begin(), oldChildren.end(), toMove),
                              oldChildren.end());
        }

        toMove->parent = targetDir;
        targetDir->children.push_back(toMove);
    }

    void FilesService::GetEntries(FSEntry *root) {
        if (root->type != EntryType::DIRECTORY) {
            return;
        }
        root->children.clear();
        for (const auto &entry: fs::directory_iterator(root->absolutePath)) {
            if (!entry.is_directory()) {
                std::string extension = entry.path().extension().string();
                if (extension.find(FILE_METADATA) != std::string::npos) {
                    DATA
                    auto sys_tp = std::chrono::file_clock::to_utc(ftime);
                    std::string dateStr = std::format("{:%Y-%m-%d %H:%M}", sys_tp);
                    auto &child = root->children.emplace_back(new FSEntry(
                        root,
                        fs::absolute(entry.path()).string(),
                        dateStr));
                    PARSE_TEMPLATE(*child, child->absolutePath.c_str())
                    child->formattedSize = FilesUtil::FormatSize(child->size);
                }
            } else {
                auto &child = root->children.emplace_back(new FSEntry(
                    root,
                    fs::absolute(entry.path()).string(),
                    ""));

                child->name = entry.path().filename().string();
                child->type = EntryType::DIRECTORY;
            }
        }
    }

} // Metal
