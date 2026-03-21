#include "IEventMember.h"
#include "ApplicationEventContext.h"

namespace Metal {
    IEventMember::~IEventMember() {
        for (int id : eventSubscriptions) {
            ApplicationEventContext::unsubscribe(id);
        }
    }

    void IEventMember::subscribeToEvent(const std::function<void(const Event &)> &callback, const std::string &key) {
        eventSubscriptions.push_back(ApplicationEventContext::subscribe(key, callback));
    }
}
