#ifndef FILEPREVIEWPANEL_H
#define FILEPREVIEWPANEL_H

#include "../../abstract/AbstractPanel.h"
#include "FilesContext.h"

namespace Metal {
    class TextureService;
    class ImGuiService;

    class FilePreviewPanel final : public AbstractPanel {
        FilesContext &filesContext;

        TextureService *textureService = nullptr;
        ImGuiService *guiContext = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"TextureService", &textureService},
                {"ImguiService", &guiContext}
            };
        }

        explicit FilePreviewPanel(FilesContext &filesContext);

        void onSync() override;
    };
}

#endif //FILEPREVIEWPANEL_H
