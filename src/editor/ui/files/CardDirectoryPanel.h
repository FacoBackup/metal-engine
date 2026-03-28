#ifndef CARDDIRECTORYPANEL_H
#define CARDDIRECTORYPANEL_H

#include "AbstractDirectoryPanel.h"

namespace Metal {
    class TextureService;
    class ImGuiService;

    class CardDirectoryPanel final : public AbstractDirectoryPanel {
        TextureService *textureService = nullptr;
        ImGuiService *guiContext = nullptr;

        void renderCard(std::shared_ptr<FSEntry> entry);

    public:
        std::vector<Dependency> getDependencies() override {
            auto deps = AbstractDirectoryPanel::getDependencies();
            deps.push_back({"TextureService", &textureService});
            deps.push_back({"ImguiService", &guiContext});
            return deps;
        }

        using AbstractDirectoryPanel::AbstractDirectoryPanel;

        void onSync() override;
    };
}

#endif //CARDDIRECTORYPANEL_H
