#ifndef METAL_ENGINE_GUICONTEXT_H
#define METAL_ENGINE_GUICONTEXT_H

#include "GLFWContext.h"

namespace Metal {
    class GUIContext {
        static void applySpacing();

        static void applyFonts();

        ApplicationContext &context;

    public:
        explicit GUIContext(ApplicationContext &context) : context(context) {
        }

        void build(bool debugMode) const;

        void shutdown() const;

        void setupContext() const;

        bool beginFrame() const;

        void endFrame() const;
    };
}

#endif
