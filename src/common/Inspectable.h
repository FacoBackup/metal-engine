#ifndef INSPECTABLE_H
#define INSPECTABLE_H

#include <functional>
#include <string>
#include <vector>
#include <glm/fwd.hpp>

#include <nlohmann/json.hpp>
#include "InspectableMember.h"
#include "Util.h"
#include "../editor/util/Util.h"

namespace Metal {
    class Inspectable {
        std::string uniqueIdentifier = Util::uuidV4();
        std::vector<std::shared_ptr<InspectableMember> > fields{};
        bool fieldsRegistered = false;

    public:
        Inspectable(const Inspectable &other)
            : uniqueIdentifier(other.uniqueIdentifier),
              fields(other.fields),
              fieldsRegistered(other.fieldsRegistered) {
        }

    protected:
        virtual void registerFields() {
        }

        void registerFloat(float &v, std::string group, std::string name,
                           float min = std::numeric_limits<float>::lowest(),
                           float max = std::numeric_limits<float>::max(),
                           bool disabled = false, float increment = .01);

        void registerInt(int &v, std::string group, std::string name,
                         int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max(),
                         bool disabled = false);

        void registerText(std::string &v, std::string group, std::string name, bool disabled = false);

        void registerBool(bool &v, std::string group, std::string name, bool disabled = false);

        void registerResourceSelection(std::string &v, std::string group, std::string name, const std::vector<std::string> &supportedFileTypes,
                                       bool disabled = false);

        void registerMethod(const std::function<void()> &updateCallback, std::string name, std::string group);

        void registerVec2(glm::vec2 &v,
                          std::string group, std::string name,
                          float min = std::numeric_limits<float>::lowest(),
                          float max = std::numeric_limits<float>::max(),
                          bool disabled = false, float increment = .01);

        void registerVec3(glm::vec3 &v,
                          std::string group, std::string name,
                          float min = std::numeric_limits<float>::lowest(),
                          float max = std::numeric_limits<float>::max(),
                          bool disabled = false, float increment = .01);

        void registerVec4(glm::vec4 &v,
                          std::string group, std::string name,
                          float min = std::numeric_limits<float>::lowest(),
                          float max = std::numeric_limits<float>::max(),
                          bool disabled = false, float increment = .01);

        void registerQuat(glm::quat &v,
                          std::string group, std::string name,
                          float min = std::numeric_limits<float>::lowest(),
                          float max = std::numeric_limits<float>::max(),
                          bool disabled = false, float increment = .01);

        void registerColor(glm::vec3 &v,
                           std::string group, std::string name, bool disabled = false);

    public:
        [[nodiscard]] nlohmann::json toJSON() const;

        [[nodiscard]] virtual std::string getClassName() const;

        std::shared_ptr<InspectableMember> getFieldByPointer(void *ptr);

        std::shared_ptr<InspectableMember> getFieldByPath(const std::string &path);

        std::vector<std::shared_ptr<InspectableMember> > &getFields();

        Inspectable() = default;

        virtual ~Inspectable() = default;

        [[nodiscard]] std::string getUniqueId() const {
            return uniqueIdentifier;
        }

        virtual const char *getIcon() const {
            throw std::logic_error("Not implemented");
        }

        virtual const char *getTitle() const {
            throw std::logic_error("Not implemented");
        }
    };
}
#endif
