#ifndef INSPECTABLE_H
#define INSPECTABLE_H

#include <functional>
#include <string>
#include <vector>
#include <glm/fwd.hpp>

#include "InspectableMember.h"
#include "../util/Util.h"
#include "../util/files/EntryType.h"

namespace Metal {
    class Inspectable {
        const std::string uniqueIdentifier = Util::uuidV4();
        std::vector<std::unique_ptr<InspectableMember> > fields;
        bool fieldsRegistered = false;
        unsigned long changes = 0;
        unsigned long frozenVersion = -1;

    protected:
        virtual void registerFields() {
        }

        void registerFloat(float &v, std::string group, std::string name, float min = std::numeric_limits<float>::min(),
                           float max = std::numeric_limits<float>::max(),
                           bool disabled = false);

        void registerInt(int &v, std::string group, std::string name,
                         int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max(),
                         bool disabled = false);

        void registerText(std::string &v, std::string group, std::string name, bool disabled = false);

        void registerBool(bool &v, std::string group, std::string name, bool disabled = false);

        void registerResourceSelection(std::string &v, std::string group, std::string name, EntryType::EntryType type,
                                       bool disabled = false);

        void registerMethod(const std::function<void()> &updateCallback, std::string name, std::string group);

        void registerVec2(glm::vec2 &v,
                          std::string group, std::string name,
                          float min = std::numeric_limits<float>::min(), float max = std::numeric_limits<float>::max(),
                          bool disabled = false);

        void registerVec3(glm::vec3 &v,
                          std::string group, std::string name,
                          float min = std::numeric_limits<float>::min(), float max = std::numeric_limits<float>::max(),
                          bool disabled = false);

        void registerVec4(glm::vec4 &v,
                          std::string group, std::string name,
                          float min = std::numeric_limits<float>::min(), float max = std::numeric_limits<float>::max(),
                          bool disabled = false);

        void registerQuat(glm::quat &v,
                          std::string group, std::string name,
                          float min = std::numeric_limits<float>::min(), float max = std::numeric_limits<float>::max(),
                          bool disabled = false);

        void registerColor(glm::vec3 &v,
                           std::string group, std::string name, bool disabled = false);

    public:
        std::vector<std::unique_ptr<InspectableMember> > &getFields();

        [[nodiscard]] unsigned long getChangeId() const;

        void registerChange();

        [[nodiscard]] bool isNotFrozen() const;

        void freezeVersion();

        virtual ~Inspectable() = default;

        [[nodiscard]] std::string getUniqueId() const {
            return uniqueIdentifier;
        }

        virtual const char *getIcon() {
            throw std::logic_error("Not implemented");
        }

        virtual const char *getTitle() {
            throw std::logic_error("Not implemented");
        }
    };
}
#endif
