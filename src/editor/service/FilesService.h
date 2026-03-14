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
        FSEntry *root = nullptr;
        DirectoryService *directoryService = nullptr;
        NotificationService *notificationService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {{"DirectoryService", directoryService}, {"NotificationService", notificationService}};
        }

        FSEntry *getRoot() const {
            return root;
        }

        void onInitialize() override;

        std::unique_ptr<FSEntry> getResource(const std::string &id);

        void deleteFiles(const std::unordered_map<std::string, FSEntry *> &files_context);

        void Move(FSEntry *toMove, FSEntry *targetDir);

        void CreateDirectory(FSEntry *currentDirectory);

        void GetEntries(FSEntry *root);
    };
} // Metal

#endif //FILESSERVICE_H
