#ifndef FILESHEADER_H
#define FILESHEADER_H
#include <functional>

#include "FilesContext.h"
#include "../../abstract/AbstractPanel.h"

namespace Metal {
    class DirectoryService;
    class FilesService;

    class FilesHeaderPanel final : public AbstractPanel {
        FilesContext &filesContext;
        DirectoryService *directoryService = nullptr;
        FilesService *filesService = nullptr;
        char addressBuffer[512] = "";

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"FilesService", &filesService},
                {"DirectoryService", &directoryService},
            };
        }

        explicit FilesHeaderPanel(FilesContext &files_context)
            : filesContext(files_context) {
        }

        void onSync() override;
    };
}

#endif
