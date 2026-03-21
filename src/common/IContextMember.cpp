#include "IContextMember.h"
#include "../ApplicationContext.h"

namespace Metal {
    void IContextMember::setDependencies(ApplicationContext &ctx) {
        this->ctx = &ctx;
        for (auto &dep : getDependencies()) {
            void* singleton = ctx.getSingletonByName(dep.name);
            if (dep.ptr != nullptr) {
                *static_cast<void**>(dep.ptr) = singleton;
            }
        }
    }
}
