#include "ApplicationContext.h"
#include <nfd.h>
#include "../common/util/files/FilesUtil.h"
#include <stdlib.h>

#include "../common/util/files/FileDialogUtil.h"

namespace Metal {
    void ApplicationContext::dispose() {
        guiContext.dispose();
        engineContext.dispose();
    }

    EngineContext &ApplicationContext::getEngineContext() {
        return engineContext;
    }

    GLFWContext &ApplicationContext::getGLFWContext() {
        return glfwContext;
    }

    VulkanContext &ApplicationContext::getVulkanContext() {
        return vulkanContext;
    }

    void ApplicationContext::updateRootPath() {
        std::string cachedPath;
        std::string cachePathFile = std::filesystem::current_path().string() + "/metal-engine-cached.txt";
        FilesUtil::ReadFile(cachePathFile.c_str(), cachedPath);

        if (cachedPath.empty()) {
            rootDirectory = FileDialogUtil::SelectDirectory();
            if (rootDirectory.empty()) {
                throw std::runtime_error("No directory selected.");
            }
            FilesUtil::WriteFile(cachePathFile.c_str(), rootDirectory.c_str());
        } else {
            rootDirectory = cachedPath;
        }
        rootDirectory.erase(std::ranges::remove(rootDirectory, '\n').begin(), rootDirectory.cend());
    }

    void ApplicationContext::start() {
        NFD_Init();

        updateRootPath();

        glfwContext.onInitialize();
        if (!glfwContext.isValidContext()) {
            throw std::runtime_error("Could not create window");
        }
        vulkanContext.onInitialize();
        guiContext.onInitialize();
        engineContext.onInitialize();
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

    bool ApplicationContext::isValidContext() const {
        return glfwContext.isValidContext();
    }

    EditorContext &ApplicationContext::getEditorContext() {
        return editorContext;
    }

    ApplicationContext::ApplicationContext(IPanel &root_panel, bool debugMode) : vulkanContext(*this, debugMode),
        rootPanel(root_panel) {
    }
}
