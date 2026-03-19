#include "IContextMember.h"
#include "../ApplicationContext.h"
#include "../editor/service/EventService.h"

namespace Metal {
    IContextMember::~IContextMember() {
        for (int id : eventSubscriptions) {
            EventService::unsubscribe(id);
        }
    }

    void IContextMember::setDependencies(ApplicationContext &ctx) {
        this->ctx = &ctx;
        for (auto &dep : getDependencies()) {
            void* singleton = ctx.getSingletonByName(dep.name);
            if (dep.ptr != nullptr) {
                *static_cast<void**>(dep.ptr) = singleton;
            }
        }
    }

    void IContextMember::subscribeToEvent(const std::function<void(const Event &)> &callback, const std::string &key) {
        eventSubscriptions.push_back(EventService::subscribe(key, callback));
    }
}
