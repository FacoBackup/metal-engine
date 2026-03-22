#ifndef METAL_ENGINE_GUICONTEXT_H
#define METAL_ENGINE_GUICONTEXT_H

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_markdown.h>
#include "../common/IService.h"
#include "../common/IDisposable.h"
#include "../common/IInit.h"

namespace Metal {
    class WindowService;
    struct FrameBufferAttachment;
    struct TextureInstance;

    class VulkanContext;
    class DescriptorSetService;

    class ImGuiService final : public IService, public IInit {
        VulkanContext *vulkanContext = nullptr;
        WindowService *windowService = nullptr;
        DescriptorSetService *descriptorSetService = nullptr;

        static void applySpacing();

        void applyFonts();

        bool swapChainRebuild = false;
        ImDrawData *drawData = nullptr;
        ImFont *largeIconsFont = nullptr;
        ImFont *h1Font = nullptr;
        ImFont *h2Font = nullptr;
        ImFont *h3Font = nullptr;

        ImGui::MarkdownConfig markdownConfig;

    public:
        [[nodiscard]] ImGui::MarkdownConfig &getMarkdownConfig() {
            return markdownConfig;
        }
        void setSwapChainRebuild(const bool val) {
            swapChainRebuild = val;
        }

        std::vector<Dependency> getDependencies() override {
            return {
                {"VulkanContext", &vulkanContext},
                {"WindowService", &windowService},
                {"DescriptorSetService", &descriptorSetService}
            };
        }

        [[ nodiscard]] ImFont *getLargeIconsFont() const {
            return largeIconsFont;
        }

        void disposeManually();

        bool beginFrame();

        void presentFrame();

        void onInitialize() override;

        void renderImage(TextureInstance *texture, float sizeX, float sizeY) const;

        static void RecordImguiCommandBuffer(ImDrawData *drawData, VkResult &err, ImGui_ImplVulkanH_Window &wd,
                                             ImGui_ImplVulkanH_Frame *fd);
    };
}

#endif
