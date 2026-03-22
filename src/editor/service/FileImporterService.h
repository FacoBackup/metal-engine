#ifndef METAL_ENGINE_LOADINGSERVICE_H
#define METAL_ENGINE_LOADINGSERVICE_H

#include <functional>
#include <stop_token>
#include <string>

#include "../dto/ImportSettingsDTO.h"
#include "../../common/IService.h"

namespace Metal {
    class SceneImporterService;
    class TextureImporterService;
    class VoxelImporterService;
    class NotificationService;
    class AsyncTaskService;

    class FileImporterService final : public IService {
        SceneImporterService *sceneImporterService = nullptr;
        TextureImporterService *textureImporterService = nullptr;
        VoxelImporterService *voxelImporterService = nullptr;
        NotificationService *notificationService = nullptr;
        AsyncTaskService *asyncTaskService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"SceneImporterService", &sceneImporterService},
                {"TextureImporterService", &textureImporterService},
                {"VoxelImporterService", &voxelImporterService},
                {"NotificationService", &notificationService},
                {"AsyncTaskService", &asyncTaskService}
            };
        }

        using LoadingTask = std::function<void(const std::stop_token &)>;

        void importFile(std::string targetDir, std::string file, const std::shared_ptr<ImportSettingsDTO> &settings);

        std::string runAsync(const std::string &taskName, const LoadingTask &task) const;

        std::string collectCompatibleFiles() const;

        bool isCompatible(const std::string &file) const;
    };
}

#endif
