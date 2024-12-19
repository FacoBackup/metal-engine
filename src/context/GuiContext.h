#ifndef METAL_ENGINE_GUICONTEXT_H
#define METAL_ENGINE_GUICONTEXT_H

#include "GLFWContext.h"
#include "../common/interface/AbstractRuntimeComponent.h"

#define LARGE_FONT_SIZE 38
namespace Metal {
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
    };
}

#endif
