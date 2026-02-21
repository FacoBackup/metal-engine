#ifndef FILESLISTPANEL_H
#define FILESLISTPANEL_H

#include "../../abstract/AbstractPanel.h"
#include "FilesContext.h"
#include <functional>
#include <imgui.h>
#include <vector>
#include <unordered_set>

namespace Metal {
    struct FileEntry;

    class FilesListPanel final : public AbstractPanel {
        FilesContext &filesContext;
        std::vector<FileEntry *> cachedSorted{};
        FileEntry *lastDir = nullptr;
        size_t lastChildrenCount = 0;
        bool sortAscending = true;
        int sortColumn = 0; // 0-Name,1-Date,2-Type,3-Size
        EntryType::EntryType typeFilter;

        bool isSomethingHovered = false;
        FileEntry *onDrag = nullptr;
        ImVec2 startDrag{-1, -1};
        std::unordered_set<std::string> loadedDirectoryPaths{};

        std::function<void(FileEntry *)> onDoubleClick;

        void ensureCache();

        void applySort();

        // Rendering helpers moved from FilesPanel
        void updateDragStart();

        void clearDragOnMouseUp();

        void handleDrag() const;

        void handleDragDrop(FileEntry *fileEntry);

        void onClick(FileEntry *root);

        void renderTreeItem(FileEntry *entry);

    public:
        explicit FilesListPanel(FilesContext &ctx, std::function<void(FileEntry *)> onDoubleClick,
                                EntryType::EntryType typeFilter)
            : filesContext(ctx), typeFilter(typeFilter), onDoubleClick(std::move(onDoubleClick)) {
        }

        void onSync() override;
    };
}

#endif //FILESLISTPANEL_H
