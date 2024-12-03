#ifndef METAL_ENGINE_APPLICATIONCONTEXT_H
#define METAL_ENGINE_APPLICATIONCONTEXT_H

#include "../engine/Engine.h"
#include "../renderer/context/GUIContext.h"

namespace Metal {
    class ApplicationContext {
    public:
        explicit ApplicationContext();

        Engine &getEngine();

        GUIContext &getGuiContext();

    private:
        Engine engine;
        IPanel *panel = nullptr;
        GUIContext guiContext;
    };

}

#endif