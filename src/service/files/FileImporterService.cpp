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
                         LOG_INFO(context, "Starting file processing: " + fileName);
                         if (context.sceneImporterService.isCompatible(file)) {
                             context.sceneImporterService.importData(targetDir,
                                                                     file, token);
                         } else if (context.textureImporter.isCompatible(file)) {
                             context.textureImporter.importData(targetDir, file, token);
                         } else if (context.voxelImporterService.isCompatible(file)) {
                             context.voxelImporterService.importData(targetDir, file, token);
                         }

                         LOG_INFO(context, "Successfully imported file: " + fileName);
                         context.notificationService.pushMessage("Successfully imported file: "  + fileName,
                                                                 NotificationSeverities::SUCCESS);
                     } catch (std::exception &e) {
                         context.notificationService.pushMessage(e.what(), NotificationSeverities::ERROR);
                     }
                 });
    }

    std::string FileImporterService::runAsync(const std::string &taskName, const LoadingTask &task) const {
        std::stop_source stopSource;
        std::string taskId = context.asyncTaskService.registerTask(taskName, [stopSource]() mutable {
            stopSource.request_stop();
        });

        std::thread([this, taskId, task, stopToken = stopSource.get_token()]() mutable {
            try {
                task(stopToken);
            } catch (const std::exception &e) {
                LOG_ERROR(context, std::string("Loading task failed: ") + e.what());
            } catch (...) {
                LOG_ERROR(context, "Loading task failed with unknown error.");
            }
            context.asyncTaskService.endTask(taskId, stopToken.stop_requested());
        }).detach();

        return taskId;
    }

    std::string FileImporterService::collectCompatibleFiles() const {
        std::string outStr = "";
        for (std::string type: context.sceneImporterService.getSupportedTypes()) {
            outStr += type + ",";
        };
        for (std::string type: context.textureImporter.getSupportedTypes()) {
            outStr += type + ",";
        };
        for (std::string type: context.voxelImporterService.getSupportedTypes()) {
            outStr += type + ",";
        };
        return outStr;
    }
}
