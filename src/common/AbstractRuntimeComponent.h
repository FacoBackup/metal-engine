#ifndef METAL_ENGINE_ABSTRACTRUNTIMECOMPONENT_H
#define METAL_ENGINE_ABSTRACTRUNTIMECOMPONENT_H

#include "interface/Initializable.h"
#include "interface/Synchornizable.h"
#define ENGINE_NAME "Metal Engine"

namespace Metal {
    class ApplicationContext;

    class AbstractRuntimeComponent : public Syncronizable, public Initializable {
    public:
        explicit AbstractRuntimeComponent(ApplicationContext &context) : context(context) {};

    protected:
        ApplicationContext &context;
    };
}

#endif
