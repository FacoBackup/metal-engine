#ifndef HISTORYEVENTSERVICE_H
#define HISTORYEVENTSERVICE_H

#include <memory>
#include <map>
#include <string>
#include <functional>

namespace Metal {
    struct EventPayload {
        virtual ~EventPayload() = default;
    };

    struct Event {
        std::string key;
        std::shared_ptr<EventPayload> payload;
    };

    class EventService final {
        struct Subscription {
            std::function<void(const Event &)> callback;
            std::string key;
        };

        static std::map<int, Subscription> subscribers;
        static int nextId;

    public:
        static int subscribe(const std::string &key, const std::function<void(const Event &)> &callback);

        static void unsubscribe(int id);

        static void dispatch(const std::string &key, const std::shared_ptr<EventPayload> &payload = nullptr);
    };
} // Metal

#endif // HISTORYEVENTSERVICE_H
