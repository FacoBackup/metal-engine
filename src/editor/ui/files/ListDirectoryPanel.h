#ifndef LISTDIRECTORYPANEL_H
#define LISTDIRECTORYPANEL_H

#include "AbstractDirectoryPanel.h"
#include <unordered_set>

namespace Metal {
    class ListDirectoryPanel final : public AbstractDirectoryPanel {
        void onClick(std::shared_ptr<FSEntry> entry);

        void renderTreeItem(std::shared_ptr<FSEntry> entry);

    public:
        using AbstractDirectoryPanel::AbstractDirectoryPanel;

        void onSync() override;
    };
}

#endif //LISTDIRECTORYPANEL_H
