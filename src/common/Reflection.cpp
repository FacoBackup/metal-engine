#include "Reflection.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <fstream>
#include <filesystem>
#include "FileExtensions.h"
#include "Util.h"
#include "LoggerUtil.h"

namespace Metal {
    FieldMetadata &Reflection::registerSerializableOnlyField(void *pointer, FieldType type, std::string name) {
        auto field = std::make_shared<FieldMetadata>();
        field->name = std::move(name);
        field->id = "##" + Util::uuidV4();
        field->nameWithId = field->name + field->id;
        field->type = type;
        field->instance = this;
        field->pointer = pointer;
        field->flags = SERIALIZABLE;
        fields.push_back(field);
        return *field;
    }

    FieldMetadata &Reflection::registerEditableField(void *pointer, FieldType type, std::string name,
                                                     std::string group) {
        auto field = std::make_shared<FieldMetadata>();
        field->name = std::move(name);
        field->id = "##" + Util::uuidV4();
        field->nameWithId = field->name + field->id;
        field->group = std::move(group);
        field->path = field->group + "/" + field->name;
        field->type = type;
        field->instance = this;
        field->pointer = pointer;
        field->flags = DEFAULT;
        fields.push_back(field);
        return *field;
    }

    std::shared_ptr<FieldMetadata> Reflection::getFieldByPointer(void *ptr) {
        if (ptr == nullptr) return nullptr;
        for (const auto &member: getFields()) {
            if (member->pointer == ptr) {
                return member;
            }
        }
        return nullptr;
    }

    std::shared_ptr<FieldMetadata> Reflection::getFieldByPath(const std::string &path) {
        for (const auto &member: getFields()) {
            if (member->path == path) {
                return member;
            }
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<FieldMetadata> > Reflection::getFields(std::optional<UsageFlags> filter) {
        if (!fieldsRegistered) {
            fieldsRegistered = true;
            registerFields();
        }

        if (!filter.has_value()) {
            return fields;
        }

        std::vector<std::shared_ptr<FieldMetadata> > filteredFields;
        filteredFields.clear();
        for (const auto &field: fields) {
            if (field->flags & filter.value()) {
                filteredFields.push_back(field);
            }
        }
        return filteredFields;
    }

    nlohmann::json Reflection::getFieldValue(UsageFlags filter, bool isVerbose,
                                             const std::vector<std::shared_ptr<FieldMetadata> >::value_type &field)
    const {
        nlohmann::json val;
        if (field->transformer) {
            val = field->transformer->toJson();
        } else if (field->type == BOOLEAN) {
            val = *static_cast<bool *>(field->pointer);
        } else if (field->type == INT) {
            val = *static_cast<int *>(field->pointer);
        } else if (field->type == FLOAT) {
            val = *static_cast<float *>(field->pointer);
        } else if (field->type == STRING || field->type == RESOURCE) {
            val = *static_cast<std::string *>(field->pointer);
        } else if (field->type == VECTOR2) {
            auto v = *static_cast<glm::vec2 *>(field->pointer);
            val = {{"x", v.x}, {"y", v.y}};
        } else if (field->type == VECTOR3 || field->type == COLOR) {
            auto v = *static_cast<glm::vec3 *>(field->pointer);
            val = {{"x", v.x}, {"y", v.y}, {"z", v.z}};
        } else if (field->type == VECTOR4) {
            auto v = *static_cast<glm::vec4 *>(field->pointer);
            val = {{"x", v.x}, {"y", v.y}, {"z", v.z}, {"w", v.w}};
        } else if (field->type == QUAT) {
            auto q = *static_cast<glm::quat *>(field->pointer);
            val = {{"x", q.x}, {"y", q.y}, {"z", q.z}, {"w", q.w}};
        } else if (field->type == COMPOSITE) {
            if (auto *reflectionInstance = dynamic_cast<Reflection *>(static_cast<Reflection *>(field->pointer))) {
                val = reflectionInstance->toJson(filter, isVerbose);
            } else {
                LOG_ERROR("Field " + field->name + " is COMPOSITE but does not implement Reflection");
                throw std::runtime_error("Field " + field->name + " is COMPOSITE but does not implement Reflection");
            }
        }
        return val;
    }

    nlohmann::json Reflection::toJson(UsageFlags filter, bool isVerbose) {
        nlohmann::json j;
        if (isVerbose) {
            j["className"] = getClassName();
            if (this->getTitle() == nullptr) {
                j["label"] = j["className"];
            } else {
                j["label"] = this->getTitle();
            }

            nlohmann::json fieldsJson = nlohmann::json::array();
            for (const auto &field: this->getFields(filter)) {
                nlohmann::json val = getFieldValue(filter, isVerbose, field);

                nlohmann::json f;
                f["name"] = field->name;
                f["group"] = field->group;
                f["type"] = field->type;
                f["path"] = field->path;
                f["value"] = val;

                if (field->type == RESOURCE) {
                    nlohmann::json supportedFileTypes = nlohmann::json::array();
                    for (const auto *info: field->supportedFileTypes) {
                        nlohmann::json i;
                        i["extension"] = info->extension;
                        i["name"] = info->name;
                        supportedFileTypes.push_back(i);
                    }
                    f["supportedFileTypes"] = supportedFileTypes;
                }
                fieldsJson.push_back(f);
            }
            j["fields"] = fieldsJson;
        } else {
            for (const auto &field: this->getFields(filter)) {
                nlohmann::json val = getFieldValue(filter, isVerbose, field);
                j[field->name] = val;
            }
        }

        return j;
    }

    std::string Reflection::getClassName() const {
        return Util::getTypeName(typeid(*this).name(), false);
    }

    void Reflection::fromJson(const nlohmann::json &j) {
        auto fields = getFields(DEFAULT);
        for (const auto &field: fields) {
            if (j.contains(field->name)) {
                const auto &val = j.at(field->name);
                applyValueFromJson(field, val);
            }
        }
    }

    void Reflection::applyValueFromJson(const std::shared_ptr<FieldMetadata> &field, const nlohmann::json &val) {
        if (field->transformer) {
            field->transformer->fromJson(val);
        } else if (field->type == BOOLEAN) {
            *static_cast<bool *>(field->pointer) = val.get<bool>();
        } else if (field->type == INT) {
            *static_cast<int *>(field->pointer) = val.get<int>();
        } else if (field->type == FLOAT) {
            *static_cast<float *>(field->pointer) = val.get<float>();
        } else if (field->type == STRING || field->type == RESOURCE) {
            *static_cast<std::string *>(field->pointer) = val.get<std::string>();
        } else if (field->type == VECTOR2) {
            auto &v = *static_cast<glm::vec2 *>(field->pointer);
            v.x = val.at("x").get<float>();
            v.y = val.at("y").get<float>();
        } else if (field->type == VECTOR3 || field->type == COLOR) {
            auto &v = *static_cast<glm::vec3 *>(field->pointer);
            v.x = val.at("x").get<float>();
            v.y = val.at("y").get<float>();
            v.z = val.at("z").get<float>();
        } else if (field->type == VECTOR4) {
            auto &v = *static_cast<glm::vec4 *>(field->pointer);
            v.x = val.at("x").get<float>();
            v.y = val.at("y").get<float>();
            v.z = val.at("z").get<float>();
            v.w = val.at("w").get<float>();
        } else if (field->type == QUAT) {
            auto &q = *static_cast<glm::quat *>(field->pointer);
            q.x = val.at("x").get<float>();
            q.y = val.at("y").get<float>();
            q.z = val.at("z").get<float>();
            q.w = val.at("w").get<float>();
        } else if (field->type == COMPOSITE) {
            if (auto *reflectionInstance = dynamic_cast<Reflection *>(static_cast<Reflection *>(field->pointer))) {
                reflectionInstance->fromJson(val);
            } else {
                LOG_ERROR("Field " + field->name + " is COMPOSITE but does not implement Reflection");
                throw std::runtime_error("Field " + field->name + " is COMPOSITE but does not implement Reflection");
            }
        }
    }
}
