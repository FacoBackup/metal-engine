#ifndef METAL_ENGINE_EDITORPANEL_H
#define METAL_ENGINE_EDITORPANEL_H

#include "abstract/AbstractPanel.h"

namespace Metal {
    class EditorPanel final : public AbstractPanel {
        AbstractPanel *pViewport = nullptr;
        AbstractPanel *pNotifications = nullptr;
        AbstractPanel *pHeader = nullptr;
        AbstractPanel *pLeftNavigation = nullptr;
        AbstractPanel *pRightNavigation = nullptr;
        AbstractPanel *pBottomNavigation = nullptr;

    public:
        void onInitialize() override;

        void renderLeftColumn();

        void renderRightColumn();

        void renderBottomColumn();

        void renderTopColumn();

        void onSync() override;
    };
}


#endif
