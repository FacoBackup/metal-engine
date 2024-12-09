#ifndef METAL_ENGINE_SYNCHORNIZABLE_H
#define METAL_ENGINE_SYNCHORNIZABLE_H

namespace Metal {
    class Syncronizable {
    public:
        virtual ~Syncronizable() = default;

        virtual void onSync() {}
    };
}

#endif
