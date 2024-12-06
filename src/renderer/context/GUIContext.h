#ifndef METAL_ENGINE_GUICONTEXT_H
#define METAL_ENGINE_GUICONTEXT_H

#include "GLFWContext.h"

namespace Metal {
    class GUIContext {
        static void applySpacing();

        static void applyFonts();

        ApplicationContext &context;
        bool isSwapChainRebuild = false;

    public:
        explicit GUIContext(ApplicationContext &context) : context(context) {
        }

        void build(bool debugMode) const;

        void frameRender(ImDrawData *draw_data);

        void renderFrame(ImDrawData *main_draw_data);

        void shutdown() const;

        void setupContext() const;

        bool beginFrame() const;

        void endFrame() ;
    };
}

#endif
