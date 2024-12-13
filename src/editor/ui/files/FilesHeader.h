#ifndef FILESHEADER_H
#define FILESHEADER_H
#include "FilesContext.h"
#include "../../common/AbstractPanel.h"

namespace Metal {
    class FilesHeader final : public AbstractPanel {
        FilesContext &filesContext;

    public:
        explicit FilesHeader(FilesContext &files_context)
            : filesContext(files_context) {
        }

        void renderOptions() const;

        void onSync() override;
    };
}

#endif
