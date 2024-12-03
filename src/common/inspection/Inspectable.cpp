#include "Inspectable.h"

#include <utility>
#include "InspectedMethod.h"

namespace Metal {
    void Inspectable::registerField(
            FieldType type, const std::function<std::any()> &getValue,
            const std::function<void(std::any)> &updateCallback,
            std::string group, std::string name,
            int min, int max, bool disabled) {
        InspectedField field;

        field.name = std::move(name);
        field.setValue = updateCallback;
        field.getValue = getValue;
        field.type = type;

        field.group = std::move(group);
        field.min = min;
        field.max = max;
        field.disabled = disabled;

        fields.push_back(field);
    }

    void Inspectable::registerField(FieldType type, const std::function<std::any()> &getValue,
                                    const std::function<void(std::any)> &updateCallback,
                                    std::string group, std::string name) {
        InspectedField field;

        field.name = std::move(name);
        field.setValue = updateCallback;
        field.getValue = getValue;
        field.type = type;

        field.group = std::move(group);
        fields.push_back(field);
    }

    void Inspectable::registerField(FieldType type, const std::function<std::any()> &getValue,
                                    const std::function<void(std::any)> &updateCallback,
                                    std::string group, std::string name, bool disabled) {
        InspectedField field;

        field.name = std::move(name);
        field.setValue = updateCallback;
        field.getValue = getValue;
        field.type = type;

        field.group = std::move(group);
        field.disabled = disabled;

        fields.push_back(field);
    }

    void Inspectable::registerMethod(const std::function<void()> &updateCallback, std::string name, std::string group) {
        InspectedMethod field;
        field.group = std::move(group);
        field.name = std::move(name);
        field.callback = updateCallback;
        fields.push_back(field);
    }

    std::vector<InspectableMember> &Inspectable::getFields() {
        if (fields.empty()) {
            registerFields();
        }
        return fields;
    }
}
