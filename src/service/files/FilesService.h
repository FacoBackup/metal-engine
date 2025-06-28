#ifndef FILESSERVICE_H
#define FILESSERVICE_H
#include <memory>
#include <string>
#include <unordered_map>

#include "../../common/AbstractRuntimeComponent.h"

namespace Metal {
    struct FileEntry;

    class FilesService final : public AbstractRuntimeComponent {
        FileEntry *root = nullptr;

    public:
        explicit FilesService(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        FileEntry *getRoot() {
            return root;
        }

        void onInitialize() override;

        std::unique_ptr<FileEntry> getResource(const std::string &id);

        void deleteFiles(const std::unordered_map<std::string, FileEntry *> &files_context);

        void createMaterial(std::string targetDir) const;

        static void Move(FileEntry *toMove, FileEntry *targetDir);

        static void GetEntries(FileEntry *root);
    };
} // Metal

#endif //FILESSERVICE_H
