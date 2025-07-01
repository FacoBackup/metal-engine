#ifndef FILESPANEL_H
#define FILESPANEL_H
#include <functional>
#include <imgui.h>

#include "FilesContext.h"
#include "../../abstract/AbstractPanel.h"

namespace Metal {
    struct FileEntry;

    class FilesPanel : public AbstractPanel {
    protected:
        static constexpr int CARD_SIZE = 90;
        static constexpr int TEXT_OFFSET = 28;
        static constexpr ImVec2 TEXTURE_SIZE{CARD_SIZE - 15, CARD_SIZE - TEXT_OFFSET - 4};

        FilesContext filesContext{nullptr};
        bool isSomethingHovered = false;
        FileEntry *onDrag = nullptr;
        ImVec2 startDrag{-1, -1};

    public:
        virtual std::string getActionLabel();

        virtual std::function<void()> onAction();

        void onInitialize() override;

        void contextMenu();

        void handleDrag() const;

        void handleDragDrop(FileEntry *fileEntry);

        void onSync() override;

        static void SetIconPos(const char *text);

        void renderItem(FileEntry *root);

        void trackDrag();

        void hotkeys();

        void pasteSelected();

        void openSelected();

        void cutSelected();

        void selectAll();

        void deleteSelected() const;

        void onClick(FileEntry *root);

        virtual void openResource(FileEntry *root);
    };
} // Metal

#endif //FILESPANEL_H
