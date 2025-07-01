#ifndef METAL_ENGINE_EDITORPANEL_H
#define METAL_ENGINE_EDITORPANEL_H

#include "abstract/AbstractPanel.h"

namespace Metal {
    class EditorPanel final : public AbstractPanel {
        AbstractPanel *pViewport = nullptr;
        AbstractPanel *pNotifications = nullptr;
        AbstractPanel *leftNavigation = nullptr;
        AbstractPanel *rightNavigation = nullptr;
        AbstractPanel *bottomNavigation = nullptr;

    public:
        void onInitialize() override;

        void renderHorizontalColumns();

        void renderVerticalColumns();

        void onSync() override;
    };
}


#endif
