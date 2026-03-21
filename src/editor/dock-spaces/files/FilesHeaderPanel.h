#ifndef FILESHEADER_H
#define FILESHEADER_H
#include <functional>

#include "FilesContext.h"
#include "../../abstract/AbstractPanel.h"

namespace Metal {
    class FilesService;

    class FilesHeaderPanel final : public AbstractPanel {
        FilesContext &filesContext;
        FilesService *filesService = nullptr;
        char addressBuffer[512] = "";

    public:
        std::vector<Dependency> getDependencies() override {
            return {{"FilesService", &filesService}};
        }

        explicit FilesHeaderPanel(FilesContext &files_context)
            : filesContext(files_context) {
        }

        void onSync() override;
    };
}

#endif
