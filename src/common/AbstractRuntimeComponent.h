#ifndef METAL_ENGINE_ABSTRACTRUNTIMECOMPONENT_H
#define METAL_ENGINE_ABSTRACTRUNTIMECOMPONENT_H

#include "interface/Changeable.h"
#include "interface/Initializable.h"
#include "interface/Synchornizable.h"

#define ENGINE_NAME "Metal Engine"

namespace Metal {
    class AbstractRuntimeComponent : public Changeable, public Syncronizable, public Initializable {
    public:
        explicit AbstractRuntimeComponent() = default;
    };
}

#endif
