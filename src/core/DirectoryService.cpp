#include "DirectoryService.h"
#include "service/SnapshotService.h"
#include "../common/FileDialogUtil.h"
#include "../common/FilesUtil.h"
#include "../common/Util.h"
#include "../common/serialization-definitions.h"
#include "../common/IRepository.h"
#include "../ApplicationContext.h"

#include "../ApplicationEventContext.h"
#include <filesystem>
#include <iomanip>
#include <sstream>
#include "editor/service/NotificationService.h"
#include "editor/repository/EditorRepository.h"
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
        engineMetadataPath = (home) ? std::string(home) + "/.metal" : ".metal";
        FilesUtil::CreateDirectory(engineMetadataPath);

        snapshotService->setEngineMetadataPath(engineMetadataPath);

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
            snapshotService->setRootDirectory(rootDirectory);
            save();
            FilesUtil::WriteFile(cachePathFile.c_str(), rootDirectory.c_str());
        } else {
            rootDirectory = cachedPath;
            snapshotService->setRootDirectory(rootDirectory);
        }
        FilesUtil::CreateDirectory(getProjectTargetPath());

        std::string projectIdPath = getProjectTargetPath() + "/project";
        if (fs::exists(projectIdPath)) {
            FilesUtil::ReadFile(projectIdPath.c_str(), projectId);
            projectId.erase(std::ranges::remove(projectId, '\n').begin(), projectId.cend());
            projectId.erase(std::ranges::remove(projectId, '\r').begin(), projectId.cend());
        }

        if (projectId.empty()) {
            projectId = Util::uuidV4();
            FilesUtil::WriteFile(projectIdPath.c_str(), projectId.c_str());
        }

        snapshotService->setProjectId(projectId);

        snapshotService->loadProject(getProjectTargetPath());

        FilesUtil::CreateDirectory(engineMetadataPath + "/shaders/");
    }


    void DirectoryService::save(const bool silent) const {
        try {
            if (rootDirectory.empty()) return;
            
            snapshotService->saveProject();

            if (!silent)
                notificationService->pushMessage("Project saved", NotificationSeverities::SUCCESS);
        } catch (const std::exception &e) {
            LOG_ERROR(e.what());
            if (!silent)
                notificationService->pushMessage("Could not save project", NotificationSeverities::ERROR);
        }
    }

    void DirectoryService::clearCaches() const {
        try {
            if (fs::exists(engineMetadataPath)) {
                for (const auto &entry: fs::directory_iterator(engineMetadataPath)) {
                    fs::remove_all(entry.path());
                }
                notificationService->pushMessage("Caches cleared", NotificationSeverities::SUCCESS);
            }
        } catch (const std::exception &e) {
            LOG_ERROR(std::string("Failed to clear caches: ") + e.what());
            notificationService->pushMessage("Could not clear caches", NotificationSeverities::ERROR);
        }
    }

    std::string DirectoryService::getRootDirectory() const {
        return rootDirectory;
    }
} // Metal
