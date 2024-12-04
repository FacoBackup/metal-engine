#ifndef METAL_ENGINE_EDITORHEADERPANEL_H
#define METAL_ENGINE_EDITORHEADERPANEL_H

#include "../../common/AbstractPanel.h"
#include "../../../common/Icons.h"

namespace Metal {

    class EditorHeaderPanel : public AbstractPanel {
        const std::string BUTTON_NEW_PROJECT = (Icons::create + "New##newProject");
        const std::string BUTTON_OPEN_PROJECT = Icons::file_open + "Open##openProject";
        const std::string BUTTON_SAVE_PROJECT = (Icons::save + "Save##save");
        const std::string BUTTON_FULL_SCREEN = Icons::fullscreen + "##fullscreen";

        ImGuiIO *io = nullptr;

        void renderFileTab();

        void framerate();

        void hotKeys();

    public:
        void onInitialize() override;

        void onSync() override;
    };

} // Metal

#endif
