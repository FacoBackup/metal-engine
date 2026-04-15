#ifndef ASSETEDITORPANEL_H
#define ASSETEDITORPANEL_H

#include "../docks/AbstractDockPanel.h"
#include "../../../engine/dto/Material.h"
#include <string>
#include <vector>
#include <memory>

namespace Metal {
    class AssetEditorPanel final : public AbstractDockPanel {
    public:
        AssetEditorPanel();

        void onSync() override;

    private:
        void loadFile(const std::string &path);

        void saveFile();

        void renderMaterialEditor();

        void renderScriptEditor();

        std::string filePath;
        std::string fileContent;
        std::string lastModified;
        char pathInput[512] = "";
        char contentBuffer[1024 * 64] = ""; // 64KB buffer for editing
        
        std::shared_ptr<Material> currentMaterial;
        bool isMaterial = false;
    };
}

#endif
