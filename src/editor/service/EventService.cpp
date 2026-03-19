#include "EventService.h"
#include "common/LoggerUtil.h"

namespace Metal {
    std::map<int, EventService::Subscription> EventService::subscribers;
    int EventService::nextId = 0;

    int EventService::subscribe(const std::string &key, const std::function<void(const Event &)> &callback) {
        int id = nextId++;
        subscribers[id] = {callback, key};
        return id;
    }

    void EventService::unsubscribe(int id) {
        subscribers.erase(id);
    }

    void EventService::dispatch(const std::string &key, const std::shared_ptr<EventPayload> &payload) {
        Event event = {key, payload};

        for (const auto &[id, sub] : subscribers) {
            if (sub.key == key) {
                sub.callback(event);
            }
        }
    }
} // Metal
