#include "Inspectable.h"

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <glm/detail/type_quat.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/quaternion.hpp>
#include "FileExtensions.h"
#include "InspectedField.h"
#include "InspectedMethod.h"
#include "Util.h"

#define  DECLARATION(T, V) \
        std::shared_ptr<InspectedField<T>> field = std::make_shared<InspectedField<T>>(&v);\
        field->name = std::move(name);\
        field->id = "##" + Util::uuidV4();\
        field->nameWithId = field->name + field->id;\
        field->group = std::move(group);\
        field->path = group + "/" + field->name;\
        field->disabled = disabled;\
        field->type = V;\
        field->instance = this;


#define  NUMERIC_DECLARATION \
        field->minF = min;\
        field->maxF = max;\
        field->incrementF = increment;\
        fields.push_back(std::move(field));

namespace Metal {
    void Inspectable::registerMethod(const std::function<void()> &updateCallback, std::string name, std::string group) {
        auto field = std::make_unique<InspectedMethod>();
        field->group = std::move(group);
        field->name = std::move(name);
        field->instance = this;
        field->nameWithId = field->name + Util::uuidV4();


        field->callback = updateCallback;
        field->type = FieldType::METHOD;
        fields.push_back(std::move(field));
    }

    std::shared_ptr<InspectableMember> Inspectable::getFieldByPointer(void *ptr) {
        if (ptr == nullptr) return nullptr;
        for (const auto &member: getFields()) {
            if (member->getGenericPointer() == ptr) {
                return member;
            }
        }
        return nullptr;
    }

    std::shared_ptr<InspectableMember> Inspectable::getFieldByPath(const std::string &path) {
        for (const auto &member: getFields()) {
            if (member->path == path) {
                return member;
            }
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<InspectableMember> > &Inspectable::getFields() {
        if (!fieldsRegistered) {
            fieldsRegistered = true;
            registerFields();
        }
        return fields;
    }

    void Inspectable::registerInt(int &v,
                                  std::string group, std::string name,
                                  int min, int max, bool disabled) {
        DECLARATION(int, INT)
        field->min = min;
        field->max = max;
        fields.push_back(std::move(field));
    }

    void Inspectable::registerText(std::string &v,
                                   std::string group, std::string name, bool disabled) {
        DECLARATION(std::string, STRING);
        fields.push_back(std::move(field));
    }

    void Inspectable::registerFloat(float &v,
                                    std::string group, std::string name,
                                    float min, float max, bool disabled, float increment) {
        DECLARATION(float, FLOAT)
        NUMERIC_DECLARATION
    }

    void Inspectable::registerVec2(glm::vec2 &v,
                                   std::string group, std::string name,
                                   float min, float max, bool disabled, float increment) {
        DECLARATION(glm::vec2, VECTOR2)
        NUMERIC_DECLARATION
    }

    void Inspectable::registerVec3(glm::vec3 &v,
                                   std::string group, std::string name,
                                   float min, float max, bool disabled, float increment) {
        DECLARATION(glm::vec3, VECTOR3)
        NUMERIC_DECLARATION
    }

    void Inspectable::registerVec4(glm::vec4 &v,
                                   std::string group, std::string name,
                                   float min, float max, bool disabled, float increment) {
        DECLARATION(glm::vec4, VECTOR4)
        NUMERIC_DECLARATION
    }

    void Inspectable::registerQuat(glm::quat &v,
                                   std::string group, std::string name,
                                   float min, float max, bool disabled, float increment) {
        DECLARATION(glm::quat, QUAT)
        NUMERIC_DECLARATION
    }

    void Inspectable::registerColor(glm::vec3 &v,
                                    std::string group, std::string name, bool disabled) {
        DECLARATION(glm::vec3, COLOR)
        fields.push_back(std::move(field));
    }

    nlohmann::json Inspectable::toJSON() const {
        nlohmann::json j;
        j["className"] = getClassName();

        try {
            j["label"] = this->getTitle();
        } catch (...) {
            j["label"] = j["className"];
        }

        nlohmann::json fieldsJson = nlohmann::json::array();

        auto &mutableThis = const_cast<Inspectable &>(*this);
        for (const auto &field: mutableThis.getFields()) {
            nlohmann::json f;
            f["name"] = field->name;
            f["group"] = field->group;
            f["type"] = field->type;
            f["path"] = field->path;

            if (field->type == FieldType::BOOLEAN) {
                f["value"] = *static_cast<InspectedField<bool> *>(field.get())->field;
            } else if (field->type == FieldType::INT) {
                f["value"] = *static_cast<InspectedField<int> *>(field.get())->field;
            } else if (field->type == FieldType::FLOAT) {
                f["value"] = *static_cast<InspectedField<float> *>(field.get())->field;
            } else if (field->type == FieldType::STRING || field->type == FieldType::RESOURCE) {
                f["value"] = *static_cast<InspectedField<std::string> *>(field.get())->field;
                if (field->type == FieldType::RESOURCE) {
                    nlohmann::json supportedFileTypes = nlohmann::json::array();
                    auto *resourceField = static_cast<InspectedField<std::string> *>(field.get());
                    for (const auto *info: resourceField->supportedFileTypes) {
                        nlohmann::json i;
                        i["extension"] = info->extension;
                        i["name"] = info->name;
                        i["typeLabel"] = info->typeLabel;
                        i["icon"] = info->icon;
                        supportedFileTypes.push_back(i);
                    }
                    f["supportedFileTypes"] = supportedFileTypes;
                }
            } else if (field->type == FieldType::VECTOR2) {
                auto v = *static_cast<InspectedField<glm::vec2> *>(field.get())->field;
                f["value"] = {{"x", v.x}, {"y", v.y}};
            } else if (field->type == FieldType::VECTOR3 || field->type == FieldType::COLOR) {
                auto v = *static_cast<InspectedField<glm::vec3> *>(field.get())->field;
                f["value"] = {{"x", v.x}, {"y", v.y}, {"z", v.z}};
            } else if (field->type == FieldType::VECTOR4) {
                auto v = *static_cast<InspectedField<glm::vec4> *>(field.get())->field;
                f["value"] = {{"x", v.x}, {"y", v.y}, {"z", v.z}, {"w", v.w}};
            } else if (field->type == FieldType::QUAT) {
                auto q = *static_cast<InspectedField<glm::quat> *>(field.get())->field;
                f["value"] = {{"x", q.x}, {"y", q.y}, {"z", q.z}, {"w", q.w}};
            }

            fieldsJson.push_back(f);
        }
        j["fields"] = fieldsJson;
        return j;
    }

    std::string Inspectable::getClassName() const {
        return Util::getTypeName(typeid(*this).name(), false);
    }

    void Inspectable::registerBool(bool &v,
                                   std::string group, std::string name, bool disabled) {
        DECLARATION(bool, BOOLEAN)
        fields.push_back(std::move(field));
    }

    void Inspectable::registerResourceSelection(std::string &v, std::string group, std::string name,
                                                const std::vector<const FileExtensionInfo *> &supportedFileTypes,
                                                bool disabled) {
        DECLARATION(std::string, RESOURCE)
        field->supportedFileTypes = supportedFileTypes;
        fields.push_back(std::move(field));
    }
}
