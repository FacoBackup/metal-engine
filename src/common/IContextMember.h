#ifndef METAL_ENGINE_CONTEXTMEMBER_H
#define METAL_ENGINE_CONTEXTMEMBER_H
#include <vector>
#include <string>
#include <functional>

namespace Metal {
    class ApplicationContext;
    struct Event;

    struct Dependency {
        std::string name;
        void *ptr;
    };

    class IContextMember {
        std::vector<int> eventSubscriptions;

    protected:
        ApplicationContext *ctx = nullptr;

    public:
        IContextMember() = default;
        virtual ~IContextMember();

        /**
         * Dependency injection. Called after all the context is created and
         * will be the data used to initialize the pointers
         */
        virtual std::vector<Dependency> getDependencies() {
            return {};
        }

        /**
         * Will call getDependencies and set the pointers based on the singletons returned
         */
        void setDependencies(ApplicationContext &ctx);

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
#endif //METAL_ENGINE_CONTEXTMEMBER_H
