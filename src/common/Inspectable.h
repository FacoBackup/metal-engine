#ifndef INSPECTABLE_H
#define INSPECTABLE_H
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "InspectedField.h"

#define MAX_INT 2147483647
namespace Metal {
    class Inspectable {
        const std::string uniqueIdentifier = typeid(this).name();
        std::vector<InspectedField> fields;

    protected:
        virtual void registerFields() {
        }

        void registerField(const std::function<void(std::any)> &updateCallback, std::string group, std::string name,
                           int min, int max, bool disabled);

        void registerField(const std::function<void(std::any)> &updateCallback, std::string group, std::string name);

        void registerField(const std::function<void(std::any)> &updateCallback, std::string group, std::string name,
                           bool disabled);

        void registerField(const std::function<void(std::any)> &updateCallback, std::string name, bool disabled);

    public:
        std::vector<InspectedField> &getFields();

        virtual ~Inspectable() = default;
    };
}
#endif
