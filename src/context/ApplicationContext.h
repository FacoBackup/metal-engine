#ifndef METAL_ENGINE_APPLICATIONCONTEXT_H
#define METAL_ENGINE_APPLICATIONCONTEXT_H
#define PROJECT_METADATA_FILE "metal-project.txt"
#include "../engine/EngineContext.h"
#include "GuiContext.h"
#include "GLFWContext.h"
#include "VulkanContext.h"
#include "../editor/common/IPanel.h"
#include "../editor/EditorContext.h"

namespace Metal {
    class ApplicationContext {
        IPanel &rootPanel;
        std::string rootDirectory;
        std::string projectName;
        GLFWContext glfwContext{*this};

    public:
        EngineContext engineContext{*this};
        EditorContext editorContext{*this};
        VulkanContext vulkanContext;
        GuiContext guiContext{*this};

        void updateRootPath(bool forceSelection);

        void updateProjectName(const std::string &projectName);

        [[nodiscard]] bool isValidContext() const {
            return glfwContext.isValidContext();
        }

        EditorContext &getEditorContext() {
            return editorContext;
        }

        EngineContext &getEngineContext() {
            return engineContext;
        }

        GLFWContext &getGLFWContext() {
            return glfwContext;
        }

        const std::string &getProjectName() {
            return projectName;
        }

        [[nodiscard]] const std::string &getRootDirectory() const {
            return rootDirectory;
        }

        [[nodiscard]] std::string getAssetRefDirectory() const {
            return rootDirectory + "/assets-ref/";
        }

        [[nodiscard]] std::string getShadersDirectory() const {
            return rootDirectory + "/shaders/";
        }

        [[nodiscard]] std::string getAssetDirectory() const {
            return rootDirectory + "/assets/";
        }

        [[nodiscard]] uint32_t getFrameIndex() const;

        void start();

        explicit ApplicationContext(IPanel &root_panel, bool debugMode);
    };
}

#endif
