#ifndef METAL_ENGINE_INITIALIZABLE_H
#define METAL_ENGINE_INITIALIZABLE_H

namespace Metal {
    class IInit {
    public:
        virtual ~IInit() = default;

        virtual void onInitialize() {}
    };
}

#endif
