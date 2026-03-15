#ifndef HISTORYEVENTSERVICE_H
#define HISTORYEVENTSERVICE_H

#include <string>
#include <functional>
#include <vector>
#include <map>
#include <algorithm>
#include <typeindex>
#include "../../common/IService.h"

namespace Metal {
    class Inspectable;

    struct HistoryEvent {
        std::string key;
        Inspectable *instance = nullptr;
        std::string propertyPath;
        std::type_index instanceType = typeid(void);
    };

    class HistoryEventService final : public IService {
        struct Subscription {
            std::function<void(const HistoryEvent &)> callback;
            std::type_index targetType = typeid(void);
            std::string targetPropertyPath;
            std::string targetKey;
            bool isGeneric = false;
        };

        std::map<int, Subscription> subscribers;
        int nextId = 0;

    public:
        int subscribe(const std::function<void(const HistoryEvent &)> &callback);

        template<typename T>
        int subscribe(const std::function<void(const HistoryEvent &)> &callback, const std::string &propertyPath = "") {
            int id = nextId++;
            subscribers[id] = {callback, typeid(T), propertyPath, "", false};
            return id;
        }

        int subscribeGeneric(const std::function<void(const HistoryEvent &)> &callback);

        void unsubscribe(int id);

        void trigger(const HistoryEvent &event);
    };
} // Metal

#endif // HISTORYEVENTSERVICE_H
