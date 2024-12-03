#ifndef INSPECTEDFIELD_H
#define INSPECTEDFIELD_H
#include <any>
#include <utility>

namespace Metal {
    struct InspectedField {
        std::function<void(std::any)> callback;
        std::string name;
        std::string group;
        int max;
        int min;
        bool disabled;
        bool hasMin = false;
        bool hasMax = false;

        InspectedField(const std::function<void(std::any)> &callback, const std::string &name, const std::string &group,
            int max, int min, bool disabled, bool has_min, bool has_max)
            : callback(callback),
              name(name),
              group(group),
              max(max),
              min(min),
              disabled(disabled),
              hasMin(has_min),
              hasMax(has_max) {
        }
    };
}
#endif
