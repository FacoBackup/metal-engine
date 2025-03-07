#ifndef METAL_ENGINE_INITIALIZABLE_H
#define METAL_ENGINE_INITIALIZABLE_H

namespace Metal {
    class Initializable {
    public:
        virtual ~Initializable() = default;

        virtual void onInitialize() {}
    };
}

#endif
