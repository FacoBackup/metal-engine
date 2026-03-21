#ifndef LUAEDITORPANEL_H
#define LUAEDITORPANEL_H

#include "../docks/AbstractDockPanel.h"
#include <string>
#include <vector>

namespace Metal {
    class LuaEditorPanel final : public AbstractDockPanel {
    public:
        LuaEditorPanel();

        void onSync() override;

    private:
        void loadFile(const std::string &path);

        void saveFile();

        std::string filePath;
        std::string fileContent;
        std::string lastModified;
        char pathInput[512] = "";
        char contentBuffer[1024 * 64] = ""; // 64KB buffer for editing
    };
}

#endif
