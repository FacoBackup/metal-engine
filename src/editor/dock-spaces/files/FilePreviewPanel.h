#ifndef FILEPREVIEWPANEL_H
#define FILEPREVIEWPANEL_H

#include "../../abstract/AbstractPanel.h"
#include "FilesContext.h"

namespace Metal {
    class FilePreviewPanel final : public AbstractPanel {
        FilesContext &filesContext;

    public:
        explicit FilePreviewPanel(FilesContext &filesContext);

        void onSync() override;
    };
}

#endif //FILEPREVIEWPANEL_H
