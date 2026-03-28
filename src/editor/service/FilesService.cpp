#include "FilesService.h"
#include "NotificationService.h"
#include "editor/dto/FSEntry.h"
#include "common/FilesUtil.h"
#include "common/LoggerUtil.h"
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include "common/serialization-definitions.h"
#include "../../core/DirectoryService.h"
#include "ApplicationContext.h"
#include "common/ILoader.h"
#include "common/AbstractImporter.h"

namespace fs = std::filesystem;

namespace Metal {
    void FilesService::onInitialize() {
        if (directoryService->getRootDirectory().empty()) {
            directoryService->updateRootPath(false);
        }
        root = std::make_shared<FSEntry>(directoryService->getRootDirectory(), "");
        root->isDirectory = true;
        root->name = "Project";
        GetEntries(root);
    }

    std::shared_ptr<FSEntry> FilesService::getResource(const std::string &id) const {
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

    void FilesService::deleteFiles(const std::unordered_map<std::string, std::shared_ptr<FSEntry> > &selected) {
        for (auto &entry: selected) {
            std::filesystem::remove_all(entry.second->absolutePath);
        }
    }

    void FilesService::Move(const std::shared_ptr<FSEntry> &toMove, const std::shared_ptr<FSEntry> &targetDir) const {
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

    void FilesService::CreateDirectory(const std::shared_ptr<FSEntry> &currentDirectory) {
        int count = 0;
        for (const auto &child: currentDirectory->children) {
            if (child->isDirectory && child->name == "New Directory (" + std::to_string(count) + ")") {
                count++;
            }
        }
        std::string pathToDir = currentDirectory->absolutePath + '/' + "New Directory (" + std::to_string(count + 1) +
                                ")";
        if (std::filesystem::exists(pathToDir)) {
            return;
        }
        std::filesystem::create_directory(pathToDir);
    }

    void FilesService::CreateFile(const std::shared_ptr<FSEntry> &currentDirectory, const std::string &name,
                                  const std::string &extension) {
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

    void FilesService::GetEntries(std::shared_ptr<FSEntry> root) {
        if (root == nullptr || !root->isDirectory) {
            return;
        }
        root->children = {};
        std::error_code ec;
        auto iterator = fs::directory_iterator(root->absolutePath, ec);
        if (ec) {
            LOG_ERROR("Error opening directory: " + root->absolutePath + " - " + ec.message());
            notificationService->pushMessage("Error opening directory", NotificationSeverities::ERROR);
            return;
        }

        for (const auto &entry: iterator) {
            std::string name = entry.path().filename().string();
            if (name.starts_with(".")) { continue; }

            std::string dateStr = "N/A";
            std::error_code date_ec;
            auto ftime = last_write_time(entry, date_ec);
            if (!date_ec) {
                auto sys_tp = std::chrono::file_clock::to_utc(ftime);
                dateStr = std::format("{:%Y-%m-%d %H:%M}", sys_tp);
            }

            auto child = std::make_shared<FSEntry>(
                fs::absolute(entry.path()).string(),
                dateStr);

            child->name = name;
            if (entry.is_directory()) {
                child->isDirectory = true;
            } else {
                std::error_code size_ec;
                child->size = fs::file_size(entry.path(), size_ec);
                if (!size_ec) {
                    child->formattedSize = FilesUtil::FormatSize(child->size);
                }
                child->extension = entry.path().extension().string();
            }
            if (canInteract(child)) {
                root->children.push_back(std::move(child));
            }
        }
    }

    bool FilesService::canInteract(const std::shared_ptr<FSEntry> &entry) const {
        if (entry->isDirectory) return true;
        const std::string &ext = entry->extension;
        for (auto *loader: ctx->getSingletons<ILoader>()) {
            if (loader->isCompatible(ext)) return true;
        }
        for (auto *importer: ctx->getSingletons<AbstractImporter>()) {
            if (importer->isCompatible(entry->absolutePath)) return true;
        }
        return false;
    }

    std::shared_ptr<FSEntry> FilesService::GetEntry(const std::string &path) {
        std::error_code ec;
        if (!fs::exists(path, ec)) {
            return nullptr;
        }

        std::string dateStr = "N/A";
        std::error_code date_ec;
        auto ftime = last_write_time(fs::path(path), date_ec);
        if (!date_ec) {
            auto sys_tp = std::chrono::file_clock::to_utc(ftime);
            dateStr = std::format("{:%Y-%m-%d %H:%M}", sys_tp);
        }

        auto entry = std::make_shared<FSEntry>(fs::absolute(path).string(), dateStr);
        entry->name = fs::path(path).filename().string();
        if (fs::is_directory(path, ec)) {
            entry->isDirectory = true;
        } else {
            std::error_code size_ec;
            entry->size = fs::file_size(path, size_ec);
            if (!size_ec) {
                entry->formattedSize = FilesUtil::FormatSize(entry->size);
            }
            entry->extension = fs::path(path).extension().string();
        }
        return entry;
    }

    std::vector<std::string> FilesService::listFilesWithExtension(const std::string &extension) const {
        std::vector<std::string> files;
        if (directoryService->getRootDirectory().empty()) {
            return files;
        }

        fs::path rootPath(directoryService->getRootDirectory());
        try {
            for (const auto &entry: fs::directory_iterator(rootPath)) {
                if (entry.is_regular_file() && entry.path().extension() == extension) {
                    files.push_back(entry.path().filename().string());
                }
            }
        } catch (const std::exception &e) {
            LOG_ERROR("Error listing files: " + std::string(e.what()));
        }
        return files;
    }

    std::string FilesService::writeRootFile(const std::string &name, const std::string &content) {
        if (!directoryService->getRootDirectory().empty()) {
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
