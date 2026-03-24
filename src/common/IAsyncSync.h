#ifndef METAL_ENGINE_IASYNCSYNC_H
#define METAL_ENGINE_IASYNCSYNC_H

#include <string>

namespace Metal {
    class IAsyncSync {
    public:
        virtual ~IAsyncSync() = default;

        virtual std::string getSyncThreadId() const = 0;

        virtual void onAsyncSync() = 0;
    };
}

#endif //METAL_ENGINE_IASYNCSYNC_H
