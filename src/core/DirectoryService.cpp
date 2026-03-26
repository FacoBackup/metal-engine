#include "DirectoryService.h"
#include "../common/FileDialogUtil.h"
#include "../common/FilesUtil.h"
#include "../common/serialization-definitions.h"
#include "../common/IRepository.h"
#include "../ApplicationContext.h"

#include <filesystem>
#include "editor/service/NotificationService.h"
#include "engine/repository/WorldRepository.h"

namespace Metal {
    std::string DirectoryService::getProjectTargetPath() const {
        return rootDirectory + "/.project";
    }

    void DirectoryService::onInitialize() {
        NFD_Init();
        updateRootPath(false);
        eventListener([this](const Event &) {
            save(true);
        }, "EditorRepository");
    }

    void DirectoryService::dispose() {
        NFD_Quit();
    }

    void DirectoryService::updateRootPath(bool forceSelection) {
        char *home = getenv("USERPROFILE");
        if (!home) home = getenv("HOME");
        if (home) {
            engineMetadataPath = std::string(home) + "/.metal";
            FilesUtil::CreateDirectory(engineMetadataPath);
        } else {
            engineMetadataPath = ".metal";
            FilesUtil::CreateDirectory(engineMetadataPath);
        }

        std::string cachedPath;
        std::string cachePathFile = engineMetadataPath + "/metal-engine-cached.txt";
        FilesUtil::ReadFile(cachePathFile.c_str(), cachedPath);
        cachedPath.erase(std::ranges::remove(cachedPath, '\n').begin(), cachedPath.cend());
        if (cachedPath.empty() || forceSelection || !fs::exists(cachedPath)) {
            rootDirectory = FileDialogUtil::SelectDirectory();
            rootDirectory.erase(std::ranges::remove(rootDirectory, '\n').begin(), rootDirectory.cend());
            if (rootDirectory.empty()) {
                throw std::runtime_error("No directory selected.");
            }
            save();
            FilesUtil::WriteFile(cachePathFile.c_str(), rootDirectory.c_str());
        } else {
            rootDirectory = cachedPath;
        }
        FilesUtil::CreateDirectory(getProjectTargetPath());
        for (auto &instance: ctx->getInstances()) {
            if (auto *repository = dynamic_cast<IRepository *>(instance.get())) {
                PARSE_TEMPLATE(*repository,
                               getProjectTargetPath() + "/" + std::to_string(std::hash<std::string>{}(typeid(*repository
                                   ).name())
                               ) + ".json")
            }
        }

        FilesUtil::CreateDirectory(engineMetadataPath + "/shaders/");
    }


    void DirectoryService::save(const bool silent) const {
        try {
            if (rootDirectory.empty()) return;
            std::string targetPath = getProjectTargetPath();
            FilesUtil::CreateDirectory(targetPath);

            for (auto &instance: ctx->getInstances()) {
                if (auto *repository = dynamic_cast<IRepository *>(instance.get())) {
                    DUMP_TEMPLATE(
                        targetPath + "/" + std::to_string(std::hash<std::string>{}(typeid(*repository).name())) +
                        ".json",
                        *repository)
                }
            }
            if (!silent)
                notificationService->pushMessage("Project saved", NotificationSeverities::SUCCESS);
        } catch (const std::exception &e) {
            LOG_ERROR(e.what());
            if (!silent)
                notificationService->pushMessage("Could not save project", NotificationSeverities::ERROR);
        }
    }

     std::string DirectoryService::getRootDirectory() const {
        return rootDirectory;
    }
} // Metal
