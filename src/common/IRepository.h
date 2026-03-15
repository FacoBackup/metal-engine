#ifndef METAL_ENGINE_IREPOSITORY_H
#define METAL_ENGINE_IREPOSITORY_H
#include "IContextMember.h"
#include "Inspectable.h"
#include "ISerialize.h"

namespace Metal {
    class IRepository : public IContextMember, public Inspectable, public ISerialize {
    public:
        virtual ~IRepository() = default;
    };
}
#endif //METAL_ENGINE_IREPOSITORY_H
