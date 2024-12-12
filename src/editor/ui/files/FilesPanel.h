#ifndef FILESPANEL_H
#define FILESPANEL_H
#include "../docks/AbstractDockPanel.h"
#include "FilesContext.h"

namespace Metal {
    class FilesPanel final : public AbstractDockPanel {
        static constexpr int CARD_SIZE = 90;
        static constexpr int TEXT_OFFSET = 28;
        static constexpr ImVec2 TEXTURE_SIZE{CARD_SIZE - 15, CARD_SIZE - TEXT_OFFSET - 4};

        std::unique_ptr<FilesContext> filesContext = nullptr;
        bool isSomethingHovered = false;

    public:
        void onInitialize() override;

        void onSync() override;

        void renderItem(FileEntry &fEntry);

        void hotkeys();

        void pasteSelected();

        void openSelected() const;

        void cutSelected() const;

        void selectAll() const;

        void deleteSelected();

        void onClick(FileEntry &root) const;

        void openResource(FileEntry &root) const;
    };
} // Metal

#endif //FILESPANEL_H
