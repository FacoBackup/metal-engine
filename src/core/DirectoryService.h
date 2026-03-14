#ifndef METAL_ENGINE_DIRECTORYSERVICE_H
#define METAL_ENGINE_DIRECTORYSERVICE_H
#include "../common/IService.h"

namespace Metal {
    struct EditorRepository;
    struct EngineRepository;
    struct WorldRepository;
    class NotificationService;

    struct DirectoryService final : IService {
        EditorRepository *editorRepository = nullptr;
        EngineRepository *engineRepository = nullptr;
        WorldRepository *worldRepository = nullptr;
        NotificationService *notificationService = nullptr;

        std::string rootDirectory;

        std::vector<Dependency> getDependencies() override {
            return {
                {"EditorRepository", editorRepository},
                {"EngineRepository", engineRepository},
                {"WorldRepository", worldRepository},
                {"NotificationService", notificationService}
            };
        }

        void updateRootPath(bool forceSelection);

        void save();

        [[nodiscard]] const std::string &getRootDirectory() const {
            return rootDirectory;
        }

        [[nodiscard]] std::string getAssetRefDirectory() const {
            return rootDirectory + "/assets-ref/";
        }

        [[nodiscard]] std::string getShadersDirectory() const {
            return rootDirectory + "/shaders/";
        }

        [[nodiscard]] std::string getAssetDirectory() const {
            return rootDirectory + "/assets/";
        }

        [[nodiscard]] std::string getVideoOutputPath() const {
            return rootDirectory + "/output.mp4";
        }
    };
} // Metal

#endif //METAL_ENGINE_DIRECTORYSERVICE_H
