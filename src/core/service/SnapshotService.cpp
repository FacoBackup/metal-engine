#include <core/service/SnapshotService.h>
#include <editor/repository/EditorRepository.h>
#include <ApplicationEventContext.h>
#include <ApplicationContext.h>
#include <common/IRepository.h>
#include <common/FilesUtil.h>
#include <common/Util.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace fs = std::filesystem;

namespace Metal {
    std::vector<Dependency> SnapshotService::getDependencies() {
        return {
            {"EditorRepository", &editorRepository}
        };
    }

    void SnapshotService::onInitialize() {
        ApplicationEventContext::subscribe("Play", [this](const Event &) {
            isPlayingSnapshotName = saveSnapshot();
            editorRepository->isPlaying = true;
        });

        ApplicationEventContext::subscribe("Stop", [this](const Event &) {
            restoreSnapshot(isPlayingSnapshotName);
            isPlayingSnapshotName.clear();
            editorRepository->isPlaying = false;
        });

        lastSnapshotTime = std::chrono::steady_clock::now();
    }

    void SnapshotService::restoreSnapshot(const std::string &name) {
        if (name.empty() || engineMetadataPath.empty() || projectId.empty()) return;
        std::string snapshotPath = engineMetadataPath + "/snapshots/" + projectId + "/" + name;
        if (!fs::exists(snapshotPath)) return;

        for (auto *instance: ctx->getSingletons<IRepository>()) {
            instance->clear();
            std::string path = snapshotPath + "/" + instance->getClassName() + ".json";
            if (fs::exists(path)) {
                std::ifstream is(path);
                if (is.is_open()) {
                    nlohmann::json j;
                    is >> j;
                    instance->fromJson(j);
                }
            }
        }
    }

    std::string SnapshotService::saveSnapshot() {
        if (engineMetadataPath.empty() || projectId.empty()) return "";
        try {
            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);

            std::stringstream ss;
            ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d-%H-%M-%S");
            std::string dateStr = ss.str();

            std::string snapshotPath = engineMetadataPath + "/snapshots/" + projectId + "/" + dateStr;
            FilesUtil::CreateDirectory(snapshotPath);

            saveProject(true, snapshotPath);

            // Cleanup: keep only the last 10 snapshots
            std::string projectSnapshotsPath = engineMetadataPath + "/snapshots/" + projectId;
            if (fs::exists(projectSnapshotsPath)) {
                std::vector<fs::path> snapshots;
                for (const auto &entry: fs::directory_iterator(projectSnapshotsPath)) {
                    if (entry.is_directory()) {
                        snapshots.push_back(entry.path());
                    }
                }

                if (snapshots.size() > 10) {
                    std::ranges::sort(snapshots);
                    for (size_t i = 0; i < snapshots.size() - 10; ++i) {
                        fs::remove_all(snapshots[i]);
                    }
                }
            }

            std::cout << "Snapshot saved: " << snapshotPath << std::endl;
            return dateStr;
        } catch (const std::exception &e) {
            LOG_ERROR(std::string("Failed to save snapshot: ") + e.what());
        }
        return "";
    }

    void SnapshotService::saveProject(bool isSnapshot, const std::string &snapshotPath) {
        if (rootDirectory.empty()) return;
        std::string targetPath = isSnapshot ? snapshotPath : rootDirectory + "/.project";
        FilesUtil::CreateDirectory(targetPath);

        for (auto *instance: ctx->getSingletons<IRepository>()) {
            std::ofstream os(targetPath + "/" + instance->getClassName() + ".json");
            if (os.is_open()) {
                std::string data = instance->toJson().dump(4);
                os << data;
            }
        }

        if (!isSnapshot) {
            saveSnapshot();
        }
    }

    void SnapshotService::loadProject(const std::string &targetPath) {
        for (auto *instance: ctx->getSingletons<IRepository>()) {
            std::string path = targetPath + "/" + instance->getClassName() + ".json";
            if (std::filesystem::exists(path)) {
                std::cout << "Loading " << path << std::endl;
                std::ifstream is(path);
                if (is.is_open()) {
                    nlohmann::json j;
                    is >> j;
                    instance->fromJson(j);
                }
            }
        }
    }

    float SnapshotService::getSyncInterval() const {
        if (!editorRepository->enableSnapshots) return 1000.f;
        return editorRepository->snapshotInterval * 60.f * 1000.f;
    }

    void SnapshotService::onAsyncSync() {
        if (!editorRepository->enableSnapshots) return;
        saveSnapshot();
    }
} // Metal
