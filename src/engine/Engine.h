#ifndef METAL_ENGINE_ENGINE_H
#define METAL_ENGINE_ENGINE_H

#include "../common/Initializable.h"
#include "../common/Synchornizable.h"

namespace Metal {
    class Engine : public Syncronizable, Initializable {
    public:
        void onInitialize() override;

        void onSync() override;
    };
}
#endif
