#ifndef FILESHEADER_H
#define FILESHEADER_H
#include "FilesContext.h"
#include "../../common/AbstractPanel.h"

namespace Metal {
    class FilesHeader final : public AbstractPanel {
        std::unique_ptr<FilesContext> &filesContext;
    public:
        explicit FilesHeader(std::unique_ptr<FilesContext> &files_context)
            : filesContext(files_context) {
        }

        void onSync() override;
    };
}

#endif
