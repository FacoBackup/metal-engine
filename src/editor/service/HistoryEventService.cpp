#include "HistoryEventService.h"

#include "common/LoggerUtil.h"

namespace Metal {
    int HistoryEventService::subscribe(const std::function<void(const HistoryEvent &)> &callback) {
        int id = nextId++;
        subscribers[id] = {callback, typeid(void), "", "", false};
        return id;
    }

    int HistoryEventService::subscribeGeneric(const std::function<void(const HistoryEvent &)> &callback) {
        int id = nextId++;
        subscribers[id] = {callback, typeid(void), "", "", true};
        return id;
    }

    void HistoryEventService::unsubscribe(int id) {
        subscribers.erase(id);
    }

    void HistoryEventService::trigger(const HistoryEvent &event) {
        for (const auto &[id, sub] : subscribers) {
            bool matches = true;

            // Generic filter
            if (sub.isGeneric) {
                if (event.key != "Undo" && event.key != "Redo" && event.key != "Action") {
                    matches = false;
                }
            } else {
                // Type filter
                if (sub.targetType != typeid(void)) {
                    if (event.instanceType != sub.targetType) {
                        matches = false;
                    }
                }

                // Property path filter
                if (matches && !sub.targetPropertyPath.empty()) {
                    if (event.propertyPath.find(sub.targetPropertyPath) == std::string::npos) {
                        matches = false;
                    }
                }

                // Key filter (if added in future)
                if (matches && !sub.targetKey.empty()) {
                    if (event.key != sub.targetKey) {
                        matches = false;
                    }
                }
            }

            if (matches) {
                LOG_INFO("Triggering event " + event.key + " for " + sub.targetType.name() + " " + sub.targetPropertyPath);
                sub.callback(event);
            }
        }
    }
} // Metal
