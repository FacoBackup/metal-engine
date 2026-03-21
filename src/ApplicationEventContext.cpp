#include "ApplicationEventContext.h"
#include <algorithm>
#include <ranges>
#include <string>
#include "common/LoggerUtil.h"

namespace Metal {
    int ApplicationEventContext::nextId = 0;

    std::map<std::string, std::vector<ApplicationEventContext::Subscription>>& ApplicationEventContext::getSubscribers() {
        static std::map<std::string, std::vector<Subscription>> subscribers;
        return subscribers;
    }

    int ApplicationEventContext::subscribe(const std::string &key, const std::function<void(const Event &)> &callback) {
        const int id = nextId++;
        getSubscribers()[key].push_back({callback, id});
        return id;
    }

    void ApplicationEventContext::unsubscribe(int id) {
        auto &subscribers = getSubscribers();
        for (auto &val: subscribers | std::views::values) {
            auto &subs = val;
            auto it = std::ranges::remove_if(subs, [id](const Subscription &sub) {
                return sub.id == id;
            }).begin();
            if (it != subs.end()) {
                subs.erase(it, subs.end());
                return;
            }
        }
    }

    void ApplicationEventContext::dispatch(const std::string &key, const std::shared_ptr<EventPayload> &payload) {
        auto &subscribers = getSubscribers();
        if (!subscribers.contains(key)) return;

        const Event event = {key, payload};
        for (auto &subs = subscribers[key]; auto &[callback, id] : subs) {
            callback(event);
        }
    }
} // Metal
