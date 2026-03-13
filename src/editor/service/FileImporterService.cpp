#include "FileImporterService.h"
#include "SceneImporterService.h"
#include "TextureImporterService.h"
#include "VoxelImporterService.h"
#include "NotificationService.h"
#include "AsyncTaskService.h"
#include "LogService.h"

#include <exception>
#include <thread>

namespace Metal {
    void FileImporterService::importFile(std::string targetDir, std::string file,
                                         const std::shared_ptr<ImportSettingsDTO> &settings) {
        std::string fileName = file.substr(file.find_last_of(std::filesystem::path::preferred_separator) + 1);
        runAsync("Import file: " + fileName,
                 [this, targetDir, file, fileName, settings](const std::stop_token &token) {
                     try {
                         LOG_INFO("Starting file processing: " + fileName);
                         if (sceneImporterService.isCompatible(file)) {
                             sceneImporterService.importData(targetDir,
                                                                 file, settings, token);
                         } else if (textureImporterService.isCompatible(file)) {
                             textureImporterService.importData(targetDir, file, settings, token);
                         } else if (voxelImporterService.isCompatible(file)) {
                             voxelImporterService.importData(targetDir, file, settings, token);
                         }

                         LOG_INFO("Successfully imported file: " + fileName);
                         notificationService.pushMessage("Successfully imported file: " + fileName,
                                                             NotificationSeverities::SUCCESS);
                     } catch (std::exception &e) {
                         notificationService.pushMessage(e.what(), NotificationSeverities::ERROR);
                     }
                 });
    }

    std::string FileImporterService::runAsync(const std::string &taskName, const LoadingTask &task) const {
        std::stop_source stopSource;
        std::string taskId = asyncTaskService.registerTask(taskName, [stopSource]() mutable {
            stopSource.request_stop();
        });

        std::thread([this, taskId, task, stopToken = stopSource.get_token()]() mutable {
            try {
                task(stopToken);
            } catch (const std::exception &e) {
                LOG_ERROR(std::string("Loading task failed: ") + e.what());
            } catch (...) {
                LOG_ERROR("Loading task failed with unknown error.");
            }
            asyncTaskService.endTask(taskId, stopToken.stop_requested());
        }).detach();

        return taskId;
    }

    std::string FileImporterService::collectCompatibleFiles() const {
        std::string outStr = "";
        for (std::string type: sceneImporterService.getSupportedTypes()) {
            outStr += type + ",";
        };
        for (std::string type: textureImporterService.getSupportedTypes()) {
            outStr += type + ",";
        };
        for (std::string type: voxelImporterService.getSupportedTypes()) {
            outStr += type + ",";
        };
        return outStr;
    }
}
