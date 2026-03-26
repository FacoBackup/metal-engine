#include "FileImporterService.h"
#include "SceneImporterService.h"
#include "TextureImporterService.h"
#include "VoxelImporterService.h"
#include "NotificationService.h"
#include "AsyncTaskService.h"
#include "common/LoggerUtil.h"

#include <exception>
#include <thread>

#include "ApplicationContext.h"
#include "common/FileExtensions.h"
#include "common/ILoader.h"
#include "core/DirectoryService.h"
#include "editor/dto/FSEntry.h"

namespace Metal {
    void FileImporterService::importFile(std::shared_ptr<FSEntry> file,
                                         const std::shared_ptr<ImportSettingsDTO> &settings) {
        const std::string ext = file->extension;
        auto loaders = ctx->getSingletons<ILoader>();
        for (auto loader: loaders) {
            if (loader->isCompatible(ext)) {
                notificationService->pushMessage("Loading...", NotificationSeverities::WARNING);
                loader->load(file->getAbsolutePath());
                return;
            }
        }

        std::string fileName = file->name.substr(
            file->absolutePath.find_last_of(std::filesystem::path::preferred_separator) + 1);
        runAsync("Import file: " + fileName,
                 [this, file, fileName, settings](const std::stop_token &token) {
                     try {
                         auto importers = ctx->getSingletons<AbstractImporter>();
                         for (auto *importer: importers) {
                             if (importer->isCompatible(file->absolutePath)) {
                                 notificationService->pushMessage("Importing...",
                                                                  NotificationSeverities::WARNING);
                                 importer->importData(directoryService->getRootDirectory(),
                                                      file->absolutePath, settings, token);

                                 notificationService->pushMessage(
                                     "Imported file to the root directory",
                                     NotificationSeverities::WARNING);
                                 return;
                             }
                         }
                         notificationService->pushMessage("Could not find importer for file type: " + fileName,
                                                          NotificationSeverities::ERROR);
                     } catch (std::exception &e) {
                         notificationService->pushMessage(e.what(), NotificationSeverities::ERROR);
                     }
                 });
    }

    std::string FileImporterService::runAsync(const std::string &taskName, const LoadingTask &task) const {
        std::stop_source stopSource;
        std::string taskId = asyncTaskService->registerTask(taskName, [stopSource]() mutable {
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
            asyncTaskService->endTask(taskId, stopToken.stop_requested());
        }).detach();

        return taskId;
    }
}
