#include "ApplicationContext.h"
#include <nfd.h>
#include "../common/util/files/FilesUtil.h"

#include "../common/util/files/FileDialogUtil.h"

namespace Metal {
    void ApplicationContext::dispose() {
        guiContext.dispose();
        vulkanContext.dispose();
    }

    void ApplicationContext::updateRootPath(bool forceSelection) {
        std::string cachedPath;
        std::string cachePathFile = std::filesystem::current_path().string() + "/metal-engine-cached.txt";
        FilesUtil::ReadFile(cachePathFile.c_str(), cachedPath);

        if (cachedPath.empty() || forceSelection) {
            rootDirectory = FileDialogUtil::SelectDirectory();
            if (rootDirectory.empty()) {
                throw std::runtime_error("No directory selected.");
            }
            FilesUtil::WriteFile(cachePathFile.c_str(), rootDirectory.c_str());
        } else {
            rootDirectory = cachedPath;
        }
        rootDirectory.erase(std::ranges::remove(rootDirectory, '\n').begin(), rootDirectory.cend());
        FilesUtil::ReadFile((rootDirectory + "/" + PROJECT_METADATA_FILE).c_str(), projectName);
        if (projectName.empty()) {
            updateProjectName("New project");
        }
        FilesUtil::MkDir(getShadersDirectory());
        FilesUtil::MkDir(getAssetRefDirectory());
        FilesUtil::MkDir(getAssetDirectory());
    }

    void ApplicationContext::updateProjectName(const std::string &projectName) {
        FilesUtil::WriteFile((rootDirectory + "/" + PROJECT_METADATA_FILE).c_str(), projectName.c_str());
        this->projectName = projectName;
    }

    void ApplicationContext::start() {
        NFD_Init();

        updateRootPath(false);

        glfwContext.onInitialize();
        if (!glfwContext.isValidContext()) {
            throw std::runtime_error("Could not create window");
        }
        vulkanContext.onInitialize();
        guiContext.onInitialize();
        editorContext.onInitialize();
        rootPanel.onInitialize();


        GLFWwindow *window = glfwContext.getWindow();
        while (!glfwWindowShouldClose(window)) {
            if (guiContext.beginFrame()) {
                continue;
            }
            rootPanel.onSync();
            engineContext.onSync();
            guiContext.endFrame();
        }
        guiContext.dispose();
        NFD_Quit();
    }

    ApplicationContext::ApplicationContext(IPanel &root_panel, bool debugMode) : vulkanContext(*this, debugMode),
        rootPanel(root_panel) {
    }
}
