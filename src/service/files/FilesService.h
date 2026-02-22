#ifndef FILESSERVICE_H
#define FILESSERVICE_H
#include <string>
#include <unordered_map>
#include <memory>

#include "../../common/AbstractRuntimeComponent.h"

namespace Metal {
    struct FSEntry;

    class FilesService final : public AbstractRuntimeComponent {
        FSEntry *root = nullptr;

    public:
        explicit FilesService()
            : AbstractRuntimeComponent() {
        }

        FSEntry *getRoot() {
            return root;
        }

        void onInitialize() override;

        std::unique_ptr<FSEntry> getResource(const std::string &id);

        void deleteFiles(const std::unordered_map<std::string, FSEntry *> &files_context);

        void createMaterial(const std::string &targetDir, FSEntry *currentDirectory) const;

        void Move(FSEntry *toMove, FSEntry *targetDir);

        static void CreateDirectory(FSEntry *currentDirectory);

        static void GetEntries(FSEntry *root);
    };
} // Metal

#endif //FILESSERVICE_H
