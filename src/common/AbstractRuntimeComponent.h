#ifndef METAL_ENGINE_ABSTRACTRUNTIMECOMPONENT_H
#define METAL_ENGINE_ABSTRACTRUNTIMECOMPONENT_H

#include "Changeable.h"
#include "Initializable.h"
#include "Synchornizable.h"

#define ENGINE_NAME "Metal Engine"

namespace Metal {
    class AbstractRuntimeComponent : public Changeable, public Syncronizable, public Initializable {
    public:
        explicit AbstractRuntimeComponent() = default;
    };
}

#endif
