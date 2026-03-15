#ifndef METAL_ENGINE_ISERVICE_H
#define METAL_ENGINE_ISERVICE_H
#include "IContextMember.h"

namespace Metal {
    class IService : public IContextMember {
    public:
        virtual ~IService() = default;
    };
}
#endif //METAL_ENGINE_ISERVICE_H
