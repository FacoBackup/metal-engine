#ifndef FILEPREVIEWPANEL_H
#define FILEPREVIEWPANEL_H

#include "../../abstract/AbstractPanel.h"
#include "FilesContext.h"

namespace Metal {
    class MaterialEditPanel;

    class FilePreviewPanel final : public AbstractPanel {
        FilesContext &filesContext;
        MaterialEditPanel *materialInspection = nullptr;

    public:
        explicit FilePreviewPanel(FilesContext &filesContext);

        void onInitialize() override;

        void onSync() override;
    };
}

#endif //FILEPREVIEWPANEL_H
