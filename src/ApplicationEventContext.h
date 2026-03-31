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

    struct MouseEventPayload final : EventPayload {
        float deltaX;
        float deltaY;

        MouseEventPayload(const float deltaX, const float deltaY) : deltaX(deltaX), deltaY(deltaY) {}
    };

    class ApplicationEventContext final {
        struct Subscription {
            std::function<void(const Event &)> callback;
            int id;
        };

        static std::map<std::string, std::vector<Subscription> >& getSubscribers();
        static int nextId;

    public:
    /**
     * Subscribe to an event. The key is case-sensitive.
     */
    static int subscribe(const std::string &key, const std::function<void(const Event &)> &callback);

    /**
     * Unsubscribe from an event using the subscription ID.
     */
    static void unsubscribe(int id);

    /**
     * Dispatch an event to all subscribers. The key is case-sensitive.
     */
    static void dispatch(const std::string &key, const std::shared_ptr<EventPayload> &payload = nullptr);
    };
} // Metal

#endif // HISTORYEVENTSERVICE_H
