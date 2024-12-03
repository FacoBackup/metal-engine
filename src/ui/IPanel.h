#ifndef METAL_ENGINE_IPANEL_H
#define METAL_ENGINE_IPANEL_H

#include "vector"

namespace Metal {
    class IPanel {
    protected:
        virtual void onInitialize() {}

    public:
        virtual void render() {}
    };
}
#endif
