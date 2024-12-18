#ifndef METAL_ENGINE_GUICONTEXT_H
#define METAL_ENGINE_GUICONTEXT_H

#include "GLFWContext.h"
#include "../common/interface/AbstractRuntimeComponent.h"

namespace Metal {
    class GuiContext final : public AbstractRuntimeComponent {
        static void applySpacing();

        static void applyFonts();

        ImDrawData *drawData = nullptr;
    public:
        explicit GuiContext(ApplicationContext &context);

        void dispose() const;

        void onInitialize() override;

        bool beginFrame() const;

        void endFrame();

    };
}

#endif
