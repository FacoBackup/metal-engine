#ifndef METAL_ENGINE_IEVENTMEMBER_H
#define METAL_ENGINE_IEVENTMEMBER_H

#include <vector>
#include <string>
#include <functional>

namespace Metal {
    struct Event;

    class IEventMember {
        std::vector<int> eventSubscriptions;

    public:
        virtual ~IEventMember();

        /**
         * Subscribe to N events with the same callback
         */
        template<typename... Args>
        void eventListener(const std::function<void(const Event &)> &callback, Args... args) {
            (subscribeToEvent(callback, args), ...);
        }

    private:
        void subscribeToEvent(const std::function<void(const Event &)> &callback, const std::string &key);
    };
}

#endif //METAL_ENGINE_IEVENTMEMBER_H
