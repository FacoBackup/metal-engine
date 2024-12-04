#ifndef METAL_ENGINE_IPANEL_H
#define METAL_ENGINE_IPANEL_H

#include "vector"
#include "../../common/Synchornizable.h"
#include "../../common/Initializable.h"

namespace Metal {
    class IPanel : public Syncronizable, public Initializable {
    public:
        virtual void removeAllChildren() {
        }
    };
}
#endif
