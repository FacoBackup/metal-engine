#ifndef METAL_ENGINE_EDITORUIRENDERER_H
#define METAL_ENGINE_EDITORUIRENDERER_H

#include "imgui.h"
#include "IUIRenderer.h"


namespace metal::ui {
    class EditorUIRenderer: public IUIRenderer{
    public:
        void render() override;
    };

}


#endif
