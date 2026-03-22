#ifndef METAL_ENGINE_DIRECTORYSERVICE_H
#define METAL_ENGINE_DIRECTORYSERVICE_H
#include "../common/IDisposable.h"
#include "../common/IInit.h"
#include "../common/IService.h"
#include "common/IEventMember.h"

namespace Metal {
    struct EditorRepository;
    struct EngineRepository;
    struct WorldRepository;
    class NotificationService;

    struct DirectoryService final : IService, IInit, IDisposable, IEventMember {
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

        void save(bool silent = false);

        [[nodiscard]] const std::string &getRootDirectory() const {
            return rootDirectory;
        }
    };
} // Metal

#endif //METAL_ENGINE_DIRECTORYSERVICE_H
