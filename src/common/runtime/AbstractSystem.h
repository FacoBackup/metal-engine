#ifndef METAL_ENGINE_ABSTRACTSYSTEM_H
#define METAL_ENGINE_ABSTRACTSYSTEM_H

#include "../Synchornizable.h"

namespace Metal {
    class ApplicationContext;

    class AbstractSystem : public Syncronizable {
    public:
         explicit AbstractSystem(ApplicationContext &context);

    protected:
        ApplicationContext &context;
    };
}

#endif
