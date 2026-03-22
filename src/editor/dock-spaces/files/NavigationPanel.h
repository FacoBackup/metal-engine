#ifndef NAVIGATIONPANEL_H
#define NAVIGATIONPANEL_H

#include "../../abstract/AbstractPanel.h"

#include "FilesContext.h"
#include "BookmarksPanel.h"
#include "SystemDirPanel.h"

namespace Metal {
    class NavigationPanel final : public AbstractPanel {
        FilesContext &filesContext;
        std::shared_ptr<BookmarksPanel> bookmarksPanel = nullptr;
        std::shared_ptr<SystemDirPanel> systemDirPanel = nullptr;

    public:
        explicit NavigationPanel(FilesContext &filesContext) : filesContext(filesContext) {}

        void onInitialize() override;

        void onSync() override;
    };
}

#endif //NAVIGATIONPANEL_H
