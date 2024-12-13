#include "FilesService.h"
#include "../../common/util/files/FileEntry.h"
#include "../../common/util/files/EntryType.h"
#include "../../common/interface/engine-definitions.h"
#include "../../common/util/files/FilesUtil.h"
#include "../../context/ApplicationContext.h"
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace Metal {
    void FilesService::onInitialize() {
        root = new FileEntry(nullptr, context.getRootDirectory(), "", "");
        root->type = EntryType::DIRECTORY;
        root->name = "Files";
        GetEntries(root);
    }

    void FilesService::Move(FileEntry *toMove,
                            FileEntry *targetDir) {
        fs::rename(toMove->absolutePath,
                   targetDir->absolutePath + "/" + toMove->absolutePath.substr(
                       toMove->absolutePath.find_last_of('/') + 1));

        for (int i = 0; i < toMove->associatedFiles.size(); i++) {
            auto file = toMove->associatedFiles[i];
            std::string newPath = targetDir->absolutePath + "/" + file.substr(file.find_last_of('/') + 1);
            fs::rename(file, newPath);
            toMove->associatedFiles[i] = newPath;
        }
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
                    auto fileSizeInBytes = fs::file_size(entry.path());
                    std::string fileSize;

                    if (fileSizeInBytes >= 1'000'000'000) {
                        fileSize = std::to_string(fileSizeInBytes / 1'000'000'000.0) + " GB";
                    } else if (fileSizeInBytes >= 1'000'000) {
                        fileSize = std::to_string(fileSizeInBytes / 1'000'000.0) + " MB";
                    } else if (fileSizeInBytes >= 1'000) {
                        fileSize = std::to_string(fileSizeInBytes / 1'000.0) + " KB";
                    } else {
                        fileSize = std::to_string(fileSizeInBytes) + " Bytes";
                    }

                    std::filesystem::file_time_type ftime = last_write_time(entry);
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
