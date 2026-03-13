#ifndef FILESSERVICE_H
#define FILESSERVICE_H
#include <string>
#include <unordered_map>
#include <memory>

#include "../../common/AbstractRuntimeComponent.h"

namespace Metal {
    struct FSEntry;

    class NotificationService;

    class FilesService final : public AbstractRuntimeComponent {
        FSEntry *root = nullptr;
        std::string &rootDirectory;
        NotificationService &notificationService;

    public:
        explicit FilesService(std::string &rootDirectory, NotificationService &notificationService)
            : AbstractRuntimeComponent(), rootDirectory(rootDirectory), notificationService(notificationService) {
        }

        FSEntry *getRoot() {
            return root;
        }

        void onInitialize() override;

        std::unique_ptr<FSEntry> getResource(const std::string &id);

        void deleteFiles(const std::unordered_map<std::string, FSEntry *> &files_context);

        void Move(FSEntry *toMove, FSEntry *targetDir);

        static void CreateDirectory(FSEntry *currentDirectory);

        static void GetEntries(FSEntry *root);
    };
} // Metal

#endif //FILESSERVICE_H
