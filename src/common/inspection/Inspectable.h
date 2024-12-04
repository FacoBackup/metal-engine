#ifndef INSPECTABLE_H
#define INSPECTABLE_H

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <limits>
#include "InspectedField.h"

#define MAX_INT std::numeric_limits<int>::max()
#define MAX_FLOAT std::numeric_limits<float>::max()

namespace Metal {
    class Inspectable {
        const std::string uniqueIdentifier = typeid(this).name();
        std::vector<std::unique_ptr<InspectableMember>> fields;
        bool fieldsRegistered = false;
    protected:
        virtual void registerFields() {
        }

        void registerFloat(float &field, std::string group, std::string name, float min, float max,
                           bool disabled);

        void registerInt(int &field, std::string group, std::string name, int min, int max,
                         bool disabled);

        void registerBool(bool &field, std::string group, std::string name, bool disabled);

        void registerMethod(const std::function<void()> &updateCallback, std::string name, std::string group);

    public:
        std::vector<std::unique_ptr<InspectableMember>> &getFields();

        virtual ~Inspectable() = default;
    };
}
#endif
