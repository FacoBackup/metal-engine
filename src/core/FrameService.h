#ifndef METAL_ENGINE_FRAMESERVICE_H
#define METAL_ENGINE_FRAMESERVICE_H
#include "../common/IDisposable.h"
#include "../common/IInit.h"
#include "../common/IService.h"

namespace Metal {
    class EngineContext;
    class VulkanContext;
    class GLFWContext;
    class GuiContext;
    class AbstractPanel;

    class FrameService final : public IService {
        GLFWContext *glfwContext = nullptr;
        VulkanContext *vulkanContext = nullptr;
        EngineContext *engineContext = nullptr;
        GuiContext *guiContext = nullptr;
        AbstractPanel *panel = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"GLFWContext", &glfwContext},
                {"VulkanContext", &vulkanContext},
                {"GuiContext", &guiContext},
                {"EngineContext", &engineContext}
            };
        }

        void setPanel(AbstractPanel *panel);

        void start() const;

        [[nodiscard]] bool isValidContext() const;

        [[nodiscard]] unsigned int getFrameIndex() const;
    };
} // Metal

#endif //METAL_ENGINE_FRAMESERVICE_H
