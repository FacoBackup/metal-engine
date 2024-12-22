#ifndef FILESSERVICE_H
#define FILESSERVICE_H
#include <string>

#include "../../common/interface/AbstractRuntimeComponent.h"
#include "../../common/util/files/FileEntry.h"

namespace Metal {
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

        void deleteFiles(std::unordered_map<std::string, FileEntry *> &files_context);

        static void Move(FileEntry *toMove, FileEntry *targetDir);

        static void GetEntries(FileEntry *root);
    };
} // Metal

#endif //FILESSERVICE_H
