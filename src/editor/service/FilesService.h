#ifndef FILESSERVICE_H
#define FILESSERVICE_H
#include <string>
#include <unordered_map>
#include <memory>

#include "../../common/IInit.h"
#include "../../common/IService.h"

namespace Metal {
    struct DirectoryService;
    struct FSEntry;

    class NotificationService;

    class FilesService final : public IService, public IInit {
        std::shared_ptr<FSEntry> root = nullptr;
        DirectoryService *directoryService = nullptr;
        NotificationService *notificationService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"DirectoryService", &directoryService},
                {"NotificationService", &notificationService}};
        }

        std::shared_ptr<FSEntry> getRoot() const {
            return root;
        }

        void onInitialize() override;

        std::shared_ptr<FSEntry> getResource(const std::string &id);

        void deleteFiles(const std::unordered_map<std::string, std::shared_ptr<FSEntry>> &files_context);

        void Move(std::shared_ptr<FSEntry> toMove, std::shared_ptr<FSEntry> targetDir);

        void CreateDirectory(std::shared_ptr<FSEntry> currentDirectory);

        void GetEntries(std::shared_ptr<FSEntry> root);

        std::shared_ptr<FSEntry> GetEntry(const std::string &path);
    };
} // Metal

#endif //FILESSERVICE_H
