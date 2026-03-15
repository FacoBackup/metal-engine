#ifndef FILESLISTPANEL_H
#define FILESLISTPANEL_H

#include "../../abstract/AbstractPanel.h"
#include "FilesContext.h"
#include <functional>
#include <imgui.h>
#include <vector>
#include <unordered_set>

namespace Metal {
    struct FSEntry;
    class FilesService;

    class FilesListPanel final : public AbstractPanel {
        FilesContext &filesContext;
        std::vector<FSEntry *> cachedSorted{};
        FSEntry *lastDir = nullptr;
        size_t lastChildrenCount = 0;
        bool sortAscending = true;
        int sortColumn = 0; // 0-Name,1-Date,2-Type,3-Size
        EntryType::EntryType typeFilter;

        bool isSomethingHovered = false;
        FSEntry *onDrag = nullptr;
        ImVec2 startDrag{-1, -1};
        std::unordered_set<std::string> loadedDirectoryPaths{};

        std::function<void(FSEntry *)> onDoubleClick;

        FilesService *filesService = nullptr;

        void ensureCache();

        void applySort();

        // Rendering helpers moved from FilesPanel
        void updateDragStart();

        void clearDragOnMouseUp();

        void handleDrag() const;

        void handleDragDrop(FSEntry *fileEntry);

        void onClick(FSEntry *root);

        void renderTreeItem(FSEntry *entry);

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"FilesService", &filesService}
            };
        }

        explicit FilesListPanel(FilesContext &filesContext, std::function<void(FSEntry *)> onDoubleClick,
                                EntryType::EntryType typeFilter)
            : filesContext(filesContext), typeFilter(typeFilter), onDoubleClick(std::move(onDoubleClick)) {
        }

        void onSync() override;
    };
}

#endif //FILESLISTPANEL_H
