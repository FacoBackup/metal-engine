#ifndef METAL_ENGINE_GUICONTEXT_H
#define METAL_ENGINE_GUICONTEXT_H

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include "../../common/IService.h"
#include "../../common/IDisposable.h"
#include "../../common/IInit.h"

namespace Metal {
    class GLFWContext;
    struct FrameBufferAttachment;
    struct TextureInstance;

    class VulkanContext;
    class DescriptorSetService;

    class GuiContext final : public IService, public IInit {
        VulkanContext *vulkanContext = nullptr;
        GLFWContext *glfwContext = nullptr;
        DescriptorSetService *descriptorSetService = nullptr;

        static void applySpacing();

        void applyFonts();

        ImDrawData *drawData = nullptr;
        ImFont *largeIconsFont = nullptr;

    public:
        GuiContext() = default;

        std::vector<Dependency> getDependencies() override {
            return {
                {"VulkanContext", &vulkanContext},
                {"GLFWContext", &glfwContext},
                {"DescriptorSetService", &descriptorSetService}
            };
        }

        [[ nodiscard]] ImFont *getLargeIconsFont() const {
            return largeIconsFont;
        }

        void disposeManually();

        void onInitialize() override;

        void endFrame();

        void renderImage(TextureInstance *texture, float sizeX, float sizeY) const;

        static void BeginFrame();

        static void RecordImguiCommandBuffer(ImDrawData *drawData, VkResult &err, ImGui_ImplVulkanH_Window &wd,
                                             ImGui_ImplVulkanH_Frame *fd);
    };
}

#endif
