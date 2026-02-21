#ifndef FILESLISTPANEL_H
#define FILESLISTPANEL_H

#include "../../abstract/AbstractPanel.h"
#include "FilesContext.h"
#include <imgui.h>
#include <vector>
#include <unordered_set>

namespace Metal {
    struct FileEntry;

    class FilesListPanel final : public AbstractPanel {
        FilesContext &filesContext;
        // Cache for sorted entries
        std::vector<FileEntry*> cachedSorted{};
        // Cache invalidation helpers
        FileEntry* lastDir = nullptr;
        size_t lastChildrenCount = 0;
        bool sortAscending = true;
        int sortColumn = 0; // 0-Name,1-Date,2-Type,3-Size

        // Drag helpers (moved from FilesPanel)
        bool isSomethingHovered = false;
        FileEntry *onDrag = nullptr;
        ImVec2 startDrag{-1, -1};
        std::unordered_set<std::string> loadedDirectoryPaths{};

        void ensureCache();
        void applySort();

        // Rendering helpers moved from FilesPanel
        void updateDragStart();
        void clearDragOnMouseUp();
        void handleDrag() const;
        void handleDragDrop(FileEntry *fileEntry);
        void onClick(FileEntry *root);

        void renderTreeItem(FileEntry *entry);

        static int columnIndexForHeader(const char* header);

    public:
        explicit FilesListPanel(FilesContext &ctx)
            : filesContext(ctx) {}

        void onSync() override;
    };
}

#endif //FILESLISTPANEL_H
