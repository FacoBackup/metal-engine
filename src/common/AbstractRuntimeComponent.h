#ifndef METAL_ENGINE_ABSTRACTRUNTIMECOMPONENT_H
#define METAL_ENGINE_ABSTRACTRUNTIMECOMPONENT_H

#include "Initializable.h"
#include "Synchornizable.h"

namespace Metal
{
    class ApplicationContext;

    class AbstractRuntimeComponent : public Syncronizable, public Initializable
    {
    };
}

#endif
