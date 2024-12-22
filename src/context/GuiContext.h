#ifndef METAL_ENGINE_GUICONTEXT_H
#define METAL_ENGINE_GUICONTEXT_H

#include "GLFWContext.h"
#include "../common/interface/AbstractRuntimeComponent.h"

#define LARGE_FONT_SIZE 38

namespace Metal {
    struct FrameBufferAttachment;
    struct TextureInstance;

    class GuiContext final : public AbstractRuntimeComponent {
        static void applySpacing();

        void applyFonts();

        ImDrawData *drawData = nullptr;

    public:
        explicit GuiContext(ApplicationContext &context);

        ImFont *largeIconsFont = nullptr;

        void dispose() const;

        void onInitialize() override;

        void endFrame();

        void renderImage(TextureInstance *texture, float sizeX, float sizeY) const;

        static void BeginFrame();

        static void RecordImguiCommandBuffer(ImDrawData *drawData, VkResult &err, ImGui_ImplVulkanH_Window &wd,
                                             ImGui_ImplVulkanH_Frame *fd);
    };
}

#endif
