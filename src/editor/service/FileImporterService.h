#ifndef METAL_ENGINE_LOADINGSERVICE_H
#define METAL_ENGINE_LOADINGSERVICE_H

#include <functional>
#include <stop_token>
#include <string>

#include "../dto/ImportSettingsDTO.h"
#include "common/IService.h"

namespace Metal {
    struct FSEntry;
    class SceneImporterService;
    class TextureImporterService;
    struct DirectoryService;
    class VoxelImporterService;
    class NotificationService;
    class AsyncTaskService;

    class FileImporterService final : public IService {
        DirectoryService *directoryService = nullptr;
        NotificationService *notificationService = nullptr;
        AsyncTaskService *asyncTaskService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"DirectoryService", &directoryService},
                {"NotificationService", &notificationService},
                {"AsyncTaskService", &asyncTaskService}
            };
        }

        using LoadingTask = std::function<void(const std::stop_token &)>;

        void importFile(std::shared_ptr<FSEntry> file, const std::shared_ptr<ImportSettingsDTO> &settings);

        std::string runAsync(const std::string &taskName, const LoadingTask &task) const;
    };
}

#endif
