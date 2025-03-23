#ifndef FILESPANEL_H
#define FILESPANEL_H
#include <imgui.h>
#include <__functional/function.h>

#include "FileItem.h"
#include "FilesContext.h"
#include "../../../../util/Util.h"
#include "../docks/AbstractDockPanel.h"

namespace fs = std::filesystem;

namespace Metal {
    struct FileEntry;

    class FilesPanel final : public AbstractDockPanel {
        std::string rootPath;
        std::string currentPath = rootPath;
        std::vector<FileItem> files;
        std::vector<std::string> directories;
        std::string selectedFile;

        std::stack<fs::path> history;
        std::stack<fs::path> forwardHistory;
        char searchBuffer[128] = "";

    public:
        void onInitialize() override;

        void onSync() override;

        void refreshDirectoryContents();

        void renderNavigationHeader();
    };
} // Metal

#endif //FILESPANEL_H
