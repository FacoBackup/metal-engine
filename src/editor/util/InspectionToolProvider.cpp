#include "InspectionToolProvider.h"
#include "../../ApplicationContext.h"
#include "../../common/IRepository.h"
#include "../../engine/repository/WorldRepository.h"
#include "../../engine/dto/Camera.h"
#include "../../engine/enum/ComponentType.h"
#include "../../common/Inspectable.h"
#include "../../common/InspectedField.h"
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Metal {
    void InspectionToolProvider::registerTools() {
        // Tool: list_repositories
        registerTool("list_repositories", "Lists all available repositories and their inspection layout", {}, 
            [this](const nlohmann::json& params) {
                nlohmann::json result = nlohmann::json::array();
                
                auto repositories = ctx->getSingletons<IRepository>();
                for (auto* repo : repositories) {
                    if (!repo->getFields().empty()) {
                        result.push_back(repo->toJSON());
                    }
                }
                return result.dump();
            });

        // Tool: get_component_info
        registerTool("get_component_info", "Returns the inspection layout for a specific component on an entity", 
            {
                {"entity_id", "The ID of the entity", ToolParamType::NUMBER},
                {"component_type_id", "The ID of the component type", ToolParamType::NUMBER}
            }, 
            [this](const nlohmann::json& params) {
                if (!params.contains("entity_id") || !params.contains("component_type_id"))
                    return std::string("Error: Missing 'entity_id' or 'component_type_id' parameter");
                
                auto entityId = params.at("entity_id").get<uint32_t>();
                auto componentTypeId = params.at("component_type_id").get<int>();

                const auto entity = static_cast<entt::entity>(entityId);
                const auto type = static_cast<ComponentType>(componentTypeId);
                
                for (const auto& def : ComponentTypes::getComponents()) {
                    if (def.type == type) {
                        if (def.hasComponent(*worldRepository, entity)) {
                            if (const Inspectable* inspectable = def.getInspectable(*worldRepository, entity)) {
                                return inspectable->toJSON().dump();
                            }
                            return std::string("Error: Component found but it's not inspectable.");
                        }
                        return std::string("Error: Entity does not have this component type.");
                    }
                }
                
                return std::string("Error: Component type not found.");
            });

        auto updateField = [](InspectableMember* member, const nlohmann::json& value) {
            if (member->type == FieldType::BOOLEAN) {
                *static_cast<InspectedField<bool>*>(member)->field = value.get<bool>();
            } else if (member->type == FieldType::INT) {
                *static_cast<InspectedField<int>*>(member)->field = value.get<int>();
            } else if (member->type == FieldType::FLOAT) {
                *static_cast<InspectedField<float>*>(member)->field = value.get<float>();
            } else if (member->type == FieldType::STRING || member->type == FieldType::RESOURCE) {
                *static_cast<InspectedField<std::string>*>(member)->field = value.get<std::string>();
            } else if (member->type == FieldType::VECTOR2) {
                auto& v = *static_cast<InspectedField<glm::vec2>*>(member)->field;
                v.x = value["x"].get<float>();
                v.y = value["y"].get<float>();
            } else if (member->type == FieldType::VECTOR3 || member->type == FieldType::COLOR) {
                auto& v = *static_cast<InspectedField<glm::vec3>*>(member)->field;
                v.x = value["x"].get<float>();
                v.y = value["y"].get<float>();
                v.z = value["z"].get<float>();
            } else if (member->type == FieldType::VECTOR4) {
                auto& v = *static_cast<InspectedField<glm::vec4>*>(member)->field;
                v.x = value["x"].get<float>();
                v.y = value["y"].get<float>();
                v.z = value["z"].get<float>();
                v.w = value["w"].get<float>();
            } else if (member->type == FieldType::QUAT) {
                auto& q = *static_cast<InspectedField<glm::quat>*>(member)->field;
                q.x = value["x"].get<float>();
                q.y = value["y"].get<float>();
                q.z = value["z"].get<float>();
                q.w = value["w"].get<float>();
            } else {
                return std::string("Error: Unsupported field type.");
            }
            return std::string("Success");
        };

        // Tool: change_component_field
        registerTool("change_component_field", "Changes a field on a specific component of an entity",
            {
                {"entity_id", "The ID of the entity", ToolParamType::NUMBER},
                {"component_type_id", "The ID of the component type", ToolParamType::NUMBER},
                {"field_path", "The path of the field to change", ToolParamType::STRING},
                {"value", "The new value for the field", ToolParamType::STRING} // Value is passed as JSON string or handled by json params
            },
            [this, updateField](const nlohmann::json& params) {
                if (!params.contains("entity_id") || !params.contains("component_type_id") || !params.contains("field_path") || !params.contains("value"))
                    return std::string("Error: Missing parameters.");

                auto entityId = params.at("entity_id").get<uint32_t>();
                auto componentTypeId = params.at("component_type_id").get<int>();
                auto fieldPath = params.at("field_path").get<std::string>();
                auto value = params.at("value");

                const auto entity = static_cast<entt::entity>(entityId);
                const auto type = static_cast<ComponentType>(componentTypeId);

                for (const auto& def : ComponentTypes::getComponents()) {
                    if (def.type == type) {
                        if (def.hasComponent(*worldRepository, entity)) {
                            if (Inspectable* inspectable = def.getInspectable(*worldRepository, entity)) {
                                auto member = inspectable->getFieldByPath(fieldPath);
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
                return std::string("Error: Component type not found.");
            });

        // Tool: change_repository_field
        registerTool("change_repository_field", "Changes a field on a specific repository",
            {
                {"repository_class_name", "The class name of the repository", ToolParamType::STRING},
                {"field_path", "The path of the field to change", ToolParamType::STRING},
                {"value", "The new value for the field", ToolParamType::STRING}
            },
            [this, updateField](const nlohmann::json& params) {
                if (!params.contains("repository_class_name") || !params.contains("field_path") || !params.contains("value"))
                    return std::string("Error: Missing parameters.");

                auto repoName = params.at("repository_class_name").get<std::string>();
                auto fieldPath = params.at("field_path").get<std::string>();
                auto value = params.at("value");

                try {
                    void* repoPtr = ctx->getSingletonByName(repoName);
                    auto* repo = static_cast<IRepository*>(repoPtr);
                    if (repo) {
                        auto member = repo->getFieldByPath(fieldPath);
                        if (member) {
                            return updateField(member.get(), value);
                        }
                        return std::string("Error: Field not found.");
                    }
                } catch (const std::exception& e) {
                    return std::string("Error: ") + e.what();
                }

                return std::string("Error: Repository not found.");
            });
    }
}
