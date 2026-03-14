#ifndef METAL_ENGINE_IPANEL_H
#define METAL_ENGINE_IPANEL_H

#include "../../common/ISync.h"
#include "../../common/IInit.h"

namespace Metal {
    class IPanel : public ISync, public IInit {
    public:
        virtual void removeAllChildren() {
        }
    };
}
#endif
