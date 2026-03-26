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

    class DirectoryService final : public IService, public IInit, public IDisposable, public IEventMember {
        NotificationService *notificationService = nullptr;

        std::string rootDirectory;
        std::string engineMetadataPath;

        std::vector<Dependency> getDependencies() override {
            return {
                {"NotificationService", &notificationService}
            };
        }

        std::string getProjectTargetPath() const;

    public:
        void onInitialize() override;

        void dispose() override;

        void updateRootPath(bool forceSelection);

        void save(bool silent = false) const;

        std::string getRootDirectory() const;

        [[nodiscard]] const std::string &getEngineMetadataPath() const {
            return engineMetadataPath;
        }
    };
} // Metal

#endif //METAL_ENGINE_DIRECTORYSERVICE_H
