#include "InspectionToolProvider.h"
#include "ApplicationContext.h"
#include "common/IRepository.h"
#include "engine/enum/ComponentType.h"
#include "common/Inspectable.h"
#include "common/InspectedField.h"
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
            if (!repo->getFields().empty()) {
                result.push_back(repo->toJSON());
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
                    if (const Inspectable *inspectable = def.getInspectable(*worldRepository, entity)) {
                        return inspectable->toJSON().dump();
                    }
                    return std::string("Error: Component found but it's not inspectable.");
                }
                return std::string("Error: Entity does not have this component type.");
            }
        }

        return std::string("Error: Component type not found.");
    }

    std::string InspectionToolProvider::changeComponentField(const nlohmann::json &params) const {
        if (!params.contains("entity_id") || !params.contains("component_type_id") || !params.contains("field_path") ||
            !params.contains("value"))
            return std::string("Error: Missing parameters.");

        auto entityId = params.at("entity_id").get<uint32_t>();
        auto componentTypeId = params.at("component_type_id").get<int>();
        auto fieldPath = params.at("field_path").get<std::string>();
        auto value = params.at("value");

        const auto entity = static_cast<entt::entity>(entityId);
        const auto type = static_cast<ComponentType>(componentTypeId);

        for (const auto &def: ComponentTypes::getComponents()) {
            if (def.type == type) {
                if (def.hasComponent(*worldRepository, entity)) {
                    if (Inspectable *inspectable = def.getInspectable(*worldRepository, entity)) {
                        const auto member = inspectable->getFieldByPath(fieldPath);
                        if (member) {
                            return updateField(member.get(), value);
                        }
                        return std::string("Error: Field not found.");
                    }
                    return std::string("Error: Component is not inspectable.");
                }
                return std::string("Error: Entity does not have this component.");
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
                const auto member = repo->getFieldByPath(fieldPath);
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

    std::string InspectionToolProvider::updateField(InspectableMember *member, const nlohmann::json &value) {
        if (member->type == BOOLEAN) {
            *dynamic_cast<InspectedField<bool> *>(member)->field = value.get<bool>();
        } else if (member->type == INT) {
            *dynamic_cast<InspectedField<int> *>(member)->field = value.get<int>();
        } else if (member->type == FLOAT) {
            *dynamic_cast<InspectedField<float> *>(member)->field = value.get<float>();
        } else if (member->type == STRING || member->type == RESOURCE) {
            *dynamic_cast<InspectedField<std::string> *>(member)->field = value.get<std::string>();
        } else if (member->type == VECTOR2) {
            auto &v = *dynamic_cast<InspectedField<glm::vec2> *>(member)->field;
            v.x = value["x"].get<float>();
            v.y = value["y"].get<float>();
        } else if (member->type == VECTOR3 || member->type == COLOR) {
            auto &v = *dynamic_cast<InspectedField<glm::vec3> *>(member)->field;
            v.x = value["x"].get<float>();
            v.y = value["y"].get<float>();
            v.z = value["z"].get<float>();
        } else if (member->type == VECTOR4) {
            auto &v = *dynamic_cast<InspectedField<glm::vec4> *>(member)->field;
            v.x = value["x"].get<float>();
            v.y = value["y"].get<float>();
            v.z = value["z"].get<float>();
            v.w = value["w"].get<float>();
        } else if (member->type == QUAT) {
            auto &q = *dynamic_cast<InspectedField<glm::quat> *>(member)->field;
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
