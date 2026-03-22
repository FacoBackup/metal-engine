#include "FilesService.h"
#include "NotificationService.h"
#include "../dto/FSEntry.h"
#include "../../common/FilesUtil.h"
#include "../../common/LoggerUtil.h"
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include "../../common/serialization-definitions.h"
#include "../../core/DirectoryService.h"

namespace fs = std::filesystem;

namespace Metal {
    void FilesService::onInitialize() {
        root = std::make_shared<FSEntry>(directoryService->getRootDirectory(), "");
        root->isDirectory = true;
        root->name = "Project";
        GetEntries(root);
    }

    std::shared_ptr<FSEntry> FilesService::getResource(const std::string &id) {
        try {
            for (const auto &entry: fs::recursive_directory_iterator(root->absolutePath)) {
                if (entry.is_regular_file() &&
                    absolute(entry.path()).string() == id) {
                    auto ftime = last_write_time(entry);
                    auto sys_tp = std::chrono::file_clock::to_utc(ftime);
                    std::string dateStr = std::format("{:%Y-%m-%d %H:%M}", sys_tp);
                    auto child = std::make_shared<FSEntry>(
                        absolute(entry.path()).string(),
                        dateStr);

                    child->name = entry.path().filename().string();
                    child->size = fs::file_size(entry.path());
                    child->formattedSize = FilesUtil::FormatSize(child->size);
                    child->extension = entry.path().extension().string();
                    return child;
                }
            }
        } catch (const fs::filesystem_error &e) {
            std::cerr << "Error while accessing directory: " << e.what() << '\n';
        }
        return nullptr;
    }

    void FilesService::deleteFiles(const std::unordered_map<std::string, std::shared_ptr<FSEntry>> &selected) {
        for (auto &entry: selected) {
            std::filesystem::remove_all(entry.second->absolutePath);
        }
    }

    void FilesService::CreateDirectory(std::shared_ptr<FSEntry> currentDirectory) {
        int count = 0;
        for (const auto &child: currentDirectory->children) {
            if (child->isDirectory && child->name == "New Directory (" + std::to_string(count) + ")") {
                count++;
            }
        }
        std::string pathToDir = currentDirectory->absolutePath + '/' + "New Directory (" + std::to_string(count + 1) + ")";
        if (std::filesystem::exists(pathToDir)) {
            return;
        }
        std::filesystem::create_directory(pathToDir);
    }

    void FilesService::CreateFile(std::shared_ptr<FSEntry> currentDirectory, const std::string &name, const std::string &extension) {
        int count = 0;
        std::string fileName = name + extension;
        while (std::filesystem::exists(currentDirectory->absolutePath + '/' + fileName)) {
            count++;
            fileName = name + " (" + std::to_string(count) + ")" + extension;
        }

        std::string pathToFile = currentDirectory->absolutePath + '/' + fileName;
        std::ofstream file(pathToFile);
        file.close();
    }

    void FilesService::Move(std::shared_ptr<FSEntry> toMove, std::shared_ptr<FSEntry> targetDir) {
        if (!targetDir || !targetDir->isDirectory) {
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

        targetDir->children.push_back(toMove);
    }

    void FilesService::GetEntries(std::shared_ptr<FSEntry> root) {
        if (!root->isDirectory) {
            return;
        }
        root->children.clear();
        for (const auto &entry: fs::directory_iterator(root->absolutePath)) {
            auto ftime = last_write_time(entry);
            auto sys_tp = std::chrono::file_clock::to_utc(ftime);
            std::string dateStr = std::format("{:%Y-%m-%d %H:%M}", sys_tp);
            auto child = std::make_shared<FSEntry>(
                fs::absolute(entry.path()).string(),
                dateStr);

            child->name = entry.path().filename().string();

            if (entry.is_directory()) {
                child->isDirectory = true;
            } else {
                child->size = fs::file_size(entry.path());
                child->formattedSize = FilesUtil::FormatSize(child->size);
                child->extension = entry.path().extension().string();
            }
            root->children.push_back(std::move(child));
        }
    }

    std::shared_ptr<FSEntry> FilesService::GetEntry(const std::string &path) {
        if (!fs::exists(path)) {
            return nullptr;
        }

        auto ftime = last_write_time(fs::path(path));
        auto sys_tp = std::chrono::file_clock::to_utc(ftime);
        std::string dateStr = std::format("{:%Y-%m-%d %H:%M}", sys_tp);

        auto entry = std::make_shared<FSEntry>(fs::absolute(path).string(), dateStr);
        entry->name = fs::path(path).filename().string();
        if (fs::is_directory(path)) {
            entry->isDirectory = true;
        } else {
            entry->size = fs::file_size(path);
            entry->formattedSize = FilesUtil::FormatSize(entry->size);
            entry->extension = fs::path(path).extension().string();
        }
        return entry;
    }

    std::vector<std::string> FilesService::listScripts() {
        std::vector<std::string> scripts;
        if (!directoryService || directoryService->getRootDirectory().empty()) {
            return scripts;
        }

        fs::path rootPath(directoryService->getRootDirectory());
        try {
            for (const auto &entry: fs::directory_iterator(rootPath)) {
                if (entry.is_regular_file() && entry.path().extension() == ".lua") {
                    scripts.push_back(entry.path().filename().string());
                }
            }
        } catch (const std::exception &e) {
            LOG_ERROR("Error listing scripts: " + std::string(e.what()));
        }
        return scripts;
    }

    std::string FilesService::writeRootFile(const std::string &name, const std::string &content) {
        if (!directoryService || directoryService->getRootDirectory().empty()) {
            return "";
        }

        fs::path rootPath(directoryService->getRootDirectory());
        fs::path filePath = rootPath / name;

        if (filePath.extension() != ".lua") {
            filePath += ".lua";
        }

        try {
            std::ofstream file(filePath);
            if (!file.is_open()) {
                return "";
            }
            file << content;
            file.close();
            return filePath.string();
        } catch (const std::exception &e) {
            LOG_ERROR("Error writing to file: " + std::string(e.what()));
            return "";
        }
    }

} // Metal
