#ifndef METAL_ENGINE_EDITORPANEL_H
#define METAL_ENGINE_EDITORPANEL_H

#include "imgui.h"
#include "common/IPanel.h"
#include "common/AbstractPanel.h"


namespace Metal {
    class EditorPanel : public AbstractPanel {
    public:
        void onInitialize() override;

        void onSync() override;
    };

}


#endif
