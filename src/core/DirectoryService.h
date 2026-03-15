#ifndef METAL_ENGINE_DIRECTORYSERVICE_H
#define METAL_ENGINE_DIRECTORYSERVICE_H
#include "../common/IDisposable.h"
#include "../common/IInit.h"
#include "../common/IService.h"

namespace Metal {
    struct EditorRepository;
    struct EngineRepository;
    struct WorldRepository;
    class NotificationService;

    struct DirectoryService final : IService, IInit, IDisposable{
        NotificationService *notificationService = nullptr;

        std::string rootDirectory;

        std::vector<Dependency> getDependencies() override {
            return {
                {"NotificationService", &notificationService}
            };
        }

        void onInitialize() override;

        void dispose() override;

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
