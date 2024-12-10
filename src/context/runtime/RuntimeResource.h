#ifndef IGPURESOURCE_H
#define IGPURESOURCE_H
#include "../../common/interface/Identifiable.h"

namespace Metal {
    class VulkanContext;

    class RuntimeResource : public Identifiable {
    public:
        virtual void dispose(VulkanContext &context) {
            throw std::runtime_error("RuntimeResource::dispose()");
        }
    };
}
#endif
