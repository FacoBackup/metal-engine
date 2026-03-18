#ifndef METAL_ENGINE_FRAMESERVICE_H
#define METAL_ENGINE_FRAMESERVICE_H
#include "../common/IDisposable.h"
#include "../common/IInit.h"
#include "../common/IService.h"

namespace Metal {
    class EngineContext;
    class VulkanContext;
    class WindowService;
    class ImGuiService;
    class AbstractPanel;

    class FrameService final : public IService {
        WindowService *windowService = nullptr;
        VulkanContext *vulkanContext = nullptr;
        EngineContext *engineContext = nullptr;
        ImGuiService *imguiService = nullptr;
        AbstractPanel *panel = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"WindowService", &windowService},
                {"VulkanContext", &vulkanContext},
                {"ImguiService", &imguiService},
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
