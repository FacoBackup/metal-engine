#ifndef FILEPREVIEWPANEL_H
#define FILEPREVIEWPANEL_H

#include "../../abstract/AbstractPanel.h"
#include "FilesContext.h"

namespace Metal {
    class TextureService;
    class GuiContext;

    class FilePreviewPanel final : public AbstractPanel {
        FilesContext &filesContext;

        TextureService *textureService = nullptr;
        GuiContext *guiContext = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"TextureService", &textureService},
                {"GuiContext", &guiContext}
            };
        }

        explicit FilePreviewPanel(FilesContext &filesContext);

        void onSync() override;
    };
}

#endif //FILEPREVIEWPANEL_H
