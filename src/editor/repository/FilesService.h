#ifndef FILESSERVICE_H
#define FILESSERVICE_H
#include <string>

#include "../../common/interface/AbstractRuntimeComponent.h"

namespace Metal {
    struct FileEntry;

    enum class EntryType;

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

        static void Move(FileEntry *toMove, FileEntry *targetDir);

        static void GetEntries(FileEntry *root);
    };
} // Metal

#endif //FILESSERVICE_H
