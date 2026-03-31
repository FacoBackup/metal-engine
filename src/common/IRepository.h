#ifndef METAL_ENGINE_IREPOSITORY_H
#define METAL_ENGINE_IREPOSITORY_H
#include "IContextMember.h"
#include "Reflection.h"

namespace Metal {
    class IRepository : public IContextMember, public Reflection {
    public:
        virtual ~IRepository() = default;

        virtual void clear() = 0;
    };
}
#endif //METAL_ENGINE_IREPOSITORY_H
