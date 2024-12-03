#include "Inspectable.h"

#include <utility>

namespace Metal {
    void Inspectable::registerField(
        const std::function<void(std::any)> &updateCallback,
        std::string group,
        std::string name,
        int min,
        int max,
        bool disabled) {
        fields.emplace_back(updateCallback, std::move(name), std::move(group), min, max, disabled, true, true);
    }

    void Inspectable::registerField(const std::function<void(std::any)> &updateCallback,
                                    std::string group, std::string name) {
        fields.emplace_back(updateCallback, std::move(name), std::move(group), 0, 0, false, false, false);
    }

    void Inspectable::registerField(const std::function<void(std::any)> &updateCallback,
                                    std::string group, std::string name, bool disabled) {
        fields.emplace_back(updateCallback, std::move(name), std::move(group), 0, 0, disabled, false, false);
    }

    void Inspectable::registerField(const std::function<void(std::any)> &updateCallback, std::string name,
                                    bool disabled) {
        fields.emplace_back(updateCallback, std::move(name), "", 0, 0, disabled, false, false);
    }

    std::vector<InspectedField> &Inspectable::getFields() {
        if (fields.empty()) {
            registerFields();
        }
        return fields;
    }
}
