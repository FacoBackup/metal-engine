#ifndef HISTORYEVENTSERVICE_H
#define HISTORYEVENTSERVICE_H

#include <memory>
#include <map>
#include <vector>
#include <string>
#include <functional>
#include "../../common/IService.h"

namespace Metal {
    struct EventPayload {
        virtual ~EventPayload() = default;
    };

    struct Event {
        std::string key;
        std::shared_ptr<EventPayload> payload;
    };

    class EventService final : public IService {
        struct Subscription {
            std::function<void(const Event &)> callback;
            std::string key;
        };

        std::map<int, Subscription> subscribers;
        int nextId = 0;

    public:
        int subscribe(const std::string &key, const std::function<void(const Event &)> &callback);

        void unsubscribe(int id);

        void dispatch(const std::string &key, const std::shared_ptr<EventPayload> &payload = nullptr);
    };
} // Metal

#endif // HISTORYEVENTSERVICE_H
