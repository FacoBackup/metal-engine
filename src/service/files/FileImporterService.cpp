#include "FileImporterService.h"

#include "../../context/ApplicationContext.h"
#include "../log/LogService.h"

#include <exception>
#include <thread>

namespace Metal {
    void FileImporterService::importFile(const std::string &targetDir, const std::string &file) const {
        std::string fileName = file.substr(file.find_last_of(std::filesystem::path::preferred_separator) + 1);
        runAsync("Import file: " + fileName,
                 [this, targetDir, file, fileName](const std::stop_token &token) {
                     try {
                         LOG_INFO("Starting file processing: " + fileName);
                         if (ApplicationContext::Get().sceneImporterService.isCompatible(file)) {
                             ApplicationContext::Get().sceneImporterService.importData(targetDir,
                                                                     file, token);
                         } else if (ApplicationContext::Get().textureImporter.isCompatible(file)) {
                             ApplicationContext::Get().textureImporter.importData(targetDir, file, token);
                         } else if (ApplicationContext::Get().voxelImporterService.isCompatible(file)) {
                             ApplicationContext::Get().voxelImporterService.importData(targetDir, file, token);
                         }

                         LOG_INFO("Successfully imported file: " + fileName);
                         ApplicationContext::Get().notificationService.pushMessage("Successfully imported file: "  + fileName,
                                                                 NotificationSeverities::SUCCESS);
                     } catch (std::exception &e) {
                         ApplicationContext::Get().notificationService.pushMessage(e.what(), NotificationSeverities::ERROR);
                     }
                 });
    }

    std::string FileImporterService::runAsync(const std::string &taskName, const LoadingTask &task) const {
        std::stop_source stopSource;
        std::string taskId = ApplicationContext::Get().asyncTaskService.registerTask(taskName, [stopSource]() mutable {
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
            ApplicationContext::Get().asyncTaskService.endTask(taskId, stopToken.stop_requested());
        }).detach();

        return taskId;
    }

    std::string FileImporterService::collectCompatibleFiles() const {
        std::string outStr = "";
        for (std::string type: ApplicationContext::Get().sceneImporterService.getSupportedTypes()) {
            outStr += type + ",";
        };
        for (std::string type: ApplicationContext::Get().textureImporter.getSupportedTypes()) {
            outStr += type + ",";
        };
        for (std::string type: ApplicationContext::Get().voxelImporterService.getSupportedTypes()) {
            outStr += type + ",";
        };
        return outStr;
    }
}
