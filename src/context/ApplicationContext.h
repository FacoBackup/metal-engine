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
        EngineContext engineContext{*this};
        EditorContext editorContext{*this};
        GLFWContext glfwContext{*this};
        VulkanContext vulkanContext;
        GuiContext guiContext{*this};
        IPanel &rootPanel;
        std::string rootDirectory;
        std::string projectName;

    public:
        void updateRootPath(bool forceSelection);

        void updateProjectName(const std::string &projectName);

        void dispose();

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

        VulkanContext &getVulkanContext() {
            return vulkanContext;
        }

        const std::string &getProjectName() {
            return projectName;
        }

        [[nodiscard]] const std::string &getRootDirectory() const {
            return rootDirectory;
        }

        void start();

        explicit ApplicationContext(IPanel &root_panel, bool debugMode);
    };
}

#endif
