#include "InspectionToolProvider.h"
#include "ApplicationContext.h"
#include "common/IRepository.h"
#include "engine/enum/ComponentType.h"
#include "common/Reflection.h"
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Metal {
    void InspectionToolProvider::registerTools() {
        // Tool: listRepositories
        registerTool("listRepositories", "Lists all available repositories and their inspection layout", {},
                     [this](const nlohmann::json &) { return listRepositories(); });

        // Tool: getComponentInfo
        registerTool("getComponentInfo", "Returns the inspection layout for a specific component on an entity",
                     {
                         {"entity_id", "The ID of the entity", ToolParamType::NUMBER},
                         {"component_type_id", "The ID of the component type", ToolParamType::NUMBER}
                     },
                     [this](const nlohmann::json &params) { return getComponentInfo(params); });

        // Tool: changeComponentField
        registerTool("changeComponentField", "Changes a field on a specific component of an entity",
                     {
                         {"entity_id", "The ID of the entity", ToolParamType::NUMBER},
                         {"component_type_id", "The ID of the component type", ToolParamType::NUMBER},
                         {"field_path", "The path of the field to change", ToolParamType::STRING},
                         {
                             "value", "The new value for the field", ToolParamType::STRING
                         } // Value is passed as JSON string or handled by json params
                     },
                     [this](const nlohmann::json &params) { return changeComponentField(params); });

        // Tool: changeRepositoryField
        registerTool("changeRepositoryField", "Changes a field on a specific repository",
                     {
                         {"repository_class_name", "The class name of the repository", ToolParamType::STRING},
                         {"field_path", "The path of the field to change", ToolParamType::STRING},
                         {"value", "The new value for the field", ToolParamType::STRING}
                     },
                     [this](const nlohmann::json &params) { return changeRepositoryField(params); });
    }

    std::string InspectionToolProvider::listRepositories() const {
        nlohmann::json result = nlohmann::json::array();

        auto repositories = ctx->getSingletons<IRepository>();
        for (auto *repo: repositories) {
            if (!repo->getFields().empty() && repo->getTitle() != nullptr) {
                result.push_back(repo->toJson(UI_VISIBLE, true));
            }
        }
        return result.dump();
    }

    std::string InspectionToolProvider::getComponentInfo(const nlohmann::json &params) const {
        if (!params.contains("entity_id") || !params.contains("component_type_id"))
            return std::string("Error: Missing 'entity_id' or 'component_type_id' parameter");

        auto entityId = params.at("entity_id").get<uint32_t>();
        auto componentTypeId = params.at("component_type_id").get<int>();

        const auto entity = static_cast<entt::entity>(entityId);
        const auto type = static_cast<ComponentType>(componentTypeId);

        for (const auto &def: ComponentTypes::getComponents()) {
            if (def.type == type) {
                if (def.hasComponent(*worldRepository, entity)) {
                    if (Reflection *reflectionInstance = def.getInspectable(*worldRepository, entity)) {
                        return reflectionInstance->toJson(UI_VISIBLE, true).dump();
                    }
                    return "Error: Component found but it's not reflectionInstance.";
                }
                return "Error: Entity does not have this component type.";
            }
        }

        return "Error: Component type not found.";
    }

    std::string InspectionToolProvider::changeComponentField(const nlohmann::json &params) const {
        if (!params.contains("entity_id") || !params.contains("component_type_id") || !params.contains("field_path") ||
            !params.contains("value"))
            return "Error: Missing parameters.";

        auto entityId = params.at("entity_id").get<uint32_t>();
        auto componentTypeId = params.at("component_type_id").get<int>();
        auto fieldPath = params.at("field_path").get<std::string>();
        auto value = params.at("value");

        const auto entity = static_cast<entt::entity>(entityId);
        const auto type = static_cast<ComponentType>(componentTypeId);

        for (const auto &def: ComponentTypes::getComponents()) {
            if (def.type == type) {
                if (def.hasComponent(*worldRepository, entity)) {
                    if (Reflection *reflectionInstance = def.getInspectable(*worldRepository, entity)) {
                        const auto member = reflectionInstance->getFieldByName(fieldPath);
                        if (member) {
                            return updateField(member.get(), value);
                        }
                        return "Error: Field not found.";
                    }
                    return "Error: Component is not reflectionInstance.";
                }
                return "Error: Entity does not have this component.";
            }
        }
        return "Error: Component type not found.";
    }

    std::string InspectionToolProvider::changeRepositoryField(const nlohmann::json &params) const {
        if (!params.contains("repository_class_name") || !params.contains("field_path") || !params.contains("value"))
            return "Error: Missing parameters.";

        const auto repoName = params.at("repository_class_name").get<std::string>();
        const auto fieldPath = params.at("field_path").get<std::string>();
        const auto &value = params.at("value");

        try {
            void *repoPtr = ctx->getSingletonByName(repoName);
            if (auto *repo = static_cast<IRepository *>(repoPtr)) {
                const auto member = repo->getFieldByName(fieldPath);
                if (member) {
                    return updateField(member.get(), value);
                }
                return "Error: Field not found.";
            }
        } catch (const std::exception &e) {
            return std::string("Error: ") + e.what();
        }

        return "Error: Repository not found.";
    }

    std::string InspectionToolProvider::updateField(FieldMetadata *member, const nlohmann::json &value) {
        if (member->type == BOOLEAN) {
            *static_cast<bool *>(member->pointer) = value.get<bool>();
        } else if (member->type == INT) {
            *static_cast<int *>(member->pointer) = value.get<int>();
        } else if (member->type == FLOAT) {
            *static_cast<float *>(member->pointer) = value.get<float>();
        } else if (member->type == STRING || member->type == RESOURCE) {
            *static_cast<std::string *>(member->pointer) = value.get<std::string>();
        } else if (member->type == VECTOR2) {
            auto &v = *static_cast<glm::vec2 *>(member->pointer);
            v.x = value["x"].get<float>();
            v.y = value["y"].get<float>();
        } else if (member->type == VECTOR3 || member->type == COLOR) {
            auto &v = *static_cast<glm::vec3 *>(member->pointer);
            v.x = value["x"].get<float>();
            v.y = value["y"].get<float>();
            v.z = value["z"].get<float>();
        } else if (member->type == VECTOR4) {
            auto &v = *static_cast<glm::vec4 *>(member->pointer);
            v.x = value["x"].get<float>();
            v.y = value["y"].get<float>();
            v.z = value["z"].get<float>();
            v.w = value["w"].get<float>();
        } else if (member->type == QUAT) {
            auto &q = *static_cast<glm::quat *>(member->pointer);
            q.x = value["x"].get<float>();
            q.y = value["y"].get<float>();
            q.z = value["z"].get<float>();
            q.w = value["w"].get<float>();
        } else {
            return "Error: Unsupported field type.";
        }
        return "Success";
    }
}
