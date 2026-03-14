#ifndef METAL_ENGINE_SYNCHORNIZABLE_H
#define METAL_ENGINE_SYNCHORNIZABLE_H

namespace Metal {
    class ISync {
    public:
        virtual ~ISync() = default;

        virtual void onSync() {}
    };
}

#endif
