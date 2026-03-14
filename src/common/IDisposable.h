#ifndef METAL_ENGINE_IDISPOSABLE_H
#define METAL_ENGINE_IDISPOSABLE_H
namespace Metal {
    class IDisposable {
    public:
        virtual ~IDisposable() = default;
        virtual void dispose() = 0;
    };
}
#endif