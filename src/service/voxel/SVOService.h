#ifndef SVOSERVICE_H
#define SVOSERVICE_H
#include "../abstract/AbstractResourceService.h"

namespace Metal {
    struct SVOInstance;
}

namespace Metal {
    class SVOService final : public AbstractResourceService{
    public:
        explicit SVOService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }

        SVOInstance *create(const std::string &id);
    };
} // Metal

#endif //SVOSERVICE_H