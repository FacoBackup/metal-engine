#ifndef INSPECTABLE_H
#define INSPECTABLE_H

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "InspectedField.h"

#define GETTER(field) [this]() { return field; }
#define SETTER(field, type) [this](const std::any &value) { field = std::any_cast<type>(value); }
#define MAX_INT 2147483647

namespace Metal {
    class Inspectable {
        const std::string uniqueIdentifier = typeid(this).name();
        std::vector<InspectableMember> fields;

    protected:
        virtual void registerFields() {
        }

        void registerField(FieldType type, const std::function<std::any()> &getValue,
                           const std::function<void(std::any)> &updateCallback, std::string group, std::string name,
                           int min, int max, bool disabled);

        void registerField(FieldType type, const std::function<std::any()> &getValue,
                           const std::function<void(std::any)> &updateCallback, std::string group, std::string name);

        void registerField(FieldType type, const std::function<std::any()> &getValue,
                           const std::function<void(std::any)> &updateCallback, std::string group, std::string name,
                           bool disabled);

        void registerMethod(const std::function<void()> &updateCallback, std::string name, std::string group);

    public:
        std::vector<InspectableMember> &getFields();

        virtual ~Inspectable() = default;
    };
}
#endif
