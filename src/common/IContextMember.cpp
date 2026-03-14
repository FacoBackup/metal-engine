#include "IContextMember.h"
#include "../ApplicationContext.h"

namespace Metal {
    void IContextMember::setDependencies(ApplicationContext &ctx) {
        this->ctx = &ctx;
        for (auto &dep : getDependencies()) {
            dep.ptr = ctx.getSingletonByName(dep.name);
        }
    }
}
