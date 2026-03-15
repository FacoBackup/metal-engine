#include "DirectoryService.h"
#include "../common/FileDialogUtil.h"
#include "../common/FilesUtil.h"
#include "../common/serialization-definitions.h"
#include "../common/IRepository.h"
#include "../ApplicationContext.h"

#include <filesystem>
#define CACHED_PATH "/metal-engine-cached.txt"
#include "../editor/service/NotificationService.h"
#include "../engine/repository/WorldRepository.h"

namespace Metal {
    void DirectoryService::onInitialize() {
        NFD_Init();
        updateRootPath(false);
    }

    void DirectoryService::dispose() {
        NFD_Quit();
    }

    void DirectoryService::updateRootPath(bool forceSelection) {
        std::string cachedPath;
        std::string cachePathFile = std::filesystem::current_path().string() + CACHED_PATH;
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

        for (auto &instance: ctx->getInstances()) {
            auto *repository = dynamic_cast<IRepository *>(instance.get());
            if (repository) {
                PARSE_TEMPLATE(*repository, rootDirectory + "/" + std::to_string(std::hash<std::string>{}(typeid(*repository).name())) + ".json")
            }
        }

        FilesUtil::CreateDirectory(getShadersDirectory());
        FilesUtil::CreateDirectory(getAssetRefDirectory());
        FilesUtil::CreateDirectory(getAssetDirectory());
    }


    void DirectoryService::save() {
        try {
            for (auto &instance: ctx->getInstances()) {
                auto *repository = dynamic_cast<IRepository *>(instance.get());
                if (repository) {
                    DUMP_TEMPLATE(rootDirectory + "/" + std::to_string(std::hash<std::string>{}(typeid(*repository).name())) + ".json",
                                  *repository)
                }
            }
            notificationService->pushMessage("Project saved", NotificationSeverities::SUCCESS);
        } catch (const std::exception &e) {
            LOG_ERROR(e.what());
            notificationService->pushMessage("Could not save project", NotificationSeverities::ERROR);
        }
    }
} // Metal
