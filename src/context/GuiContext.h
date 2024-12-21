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

        bool beginFrame() const;

        void endFrame();

        void renderImage(FrameBufferAttachment *attachment, float sizeX, float sizeY) const;

        void renderImage(TextureInstance *texture, float sizeX, float sizeY) const;
    };
}

#endif
