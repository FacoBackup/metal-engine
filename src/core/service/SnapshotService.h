#ifndef METAL_ENGINE_SNAPSHOTSERVICE_H
#define METAL_ENGINE_SNAPSHOTSERVICE_H

#include <common/IService.h>
#include <common/IAsyncSync.h>
#include <common/IInit.h>
#include <string>
#include <vector>
#include <chrono>

namespace Metal {
    struct EditorRepository;

    class SnapshotService final : public IService, public IInit, public IAsyncSync {
        EditorRepository *editorRepository = nullptr;

        std::string isPlayingSnapshotName;
        std::chrono::steady_clock::time_point lastSnapshotTime;
        std::string engineMetadataPath;
        std::string projectId;
        std::string rootDirectory;

    public:
        std::vector<Dependency> getDependencies() override;

        void onInitialize() override;

        void restoreSnapshot(const std::string &name);

        std::string saveSnapshot();

        void saveProject(bool isSnapshot = false, const std::string &snapshotPath = "");

        void loadProject(const std::string &targetPath);

        void setEngineMetadataPath(const std::string &path) { engineMetadataPath = path; }

        void setProjectId(const std::string &id) { projectId = id; }

        void setRootDirectory(const std::string &path) { rootDirectory = path; }

        [[nodiscard]] const std::string &getEngineMetadataPath() const { return engineMetadataPath; }

        [[nodiscard]] const std::string &getProjectId() const { return projectId; }

        std::string getSyncThreadId() const override {
            return "Background";
        }

        float getSyncInterval() const override;

        void onAsyncSync() override;
    };
} // Metal

#endif //METAL_ENGINE_SNAPSHOTSERVICE_H
