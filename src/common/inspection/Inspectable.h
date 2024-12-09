#ifndef INSPECTABLE_H
#define INSPECTABLE_H

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "InspectedField.h"

namespace Metal {
    class Inspectable {
        const std::string uniqueIdentifier = typeid(this).name();
        std::vector<std::unique_ptr<InspectableMember> > fields;
        bool fieldsRegistered = false;
        int changes = 0;
        int frozenVersion = -1;

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
        std::vector<std::unique_ptr<InspectableMember> > &getFields();

        [[nodiscard]] int getChangeId() const;

        void registerChange();

        [[nodiscard]] bool isNotFrozen() const;

        void freezeVersion();

        virtual ~Inspectable() = default;
    };
}
#endif
