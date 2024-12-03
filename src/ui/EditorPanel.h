#ifndef METAL_ENGINE_EDITORPANEL_H
#define METAL_ENGINE_EDITORPANEL_H

#include "imgui.h"
#include "IPanel.h"
#include "AbstractPanel.h"


namespace Metal {
    class EditorPanel: public AbstractPanel{
    public:
        void render() override;
    };

}


#endif
