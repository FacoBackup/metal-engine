#ifndef METAL_ENGINE_DIRECTORYSERVICE_H
#define METAL_ENGINE_DIRECTORYSERVICE_H
#include "../common/IDisposable.h"
#include "../common/IInit.h"
#include "../common/IService.h"
#include "../common/IAsyncSync.h"
#include "common/IEventMember.h"
#include <chrono>

namespace Metal {
    struct EditorRepository;
    struct EngineRepository;
    struct WorldRepository;
    class NotificationService;

    class SnapshotService;

    class DirectoryService final : public IService, public IInit, public IDisposable, public IEventMember {
        NotificationService *notificationService = nullptr;
        EditorRepository *editorRepository = nullptr;
        SnapshotService *snapshotService = nullptr;

        std::string rootDirectory;
        std::string engineMetadataPath;
        std::string projectId;

        std::vector<Dependency> getDependencies() override {
            return {
                {"NotificationService", &notificationService},
                {"EditorRepository", &editorRepository},
                {"SnapshotService", &snapshotService}
            };
        }

        std::string getProjectTargetPath() const;

    public:
        void onInitialize() override;

        void dispose() override;

        void updateRootPath(bool forceSelection);

        void save(bool silent = false) const;

        void clearCaches() const;

        std::string getRootDirectory() const;

        [[nodiscard]] const std::string &getEngineMetadataPath() const {
            return engineMetadataPath;
        }

        [[nodiscard]] const std::string &getProjectId() const {
            return projectId;
        }
    };
} // Metal

#endif //METAL_ENGINE_DIRECTORYSERVICE_H
