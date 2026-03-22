#include "WorldToolProvider.h"
#include "../../engine/repository/WorldRepository.h"
#include "../../engine/enum/ComponentType.h"
#include "../../engine/dto/MetadataComponent.h"
#include <sstream>

namespace Metal {
    void WorldToolProvider::registerTools() {
        // Tool: create_entity
        registerTool("create_entity", "Creates a new entity in the world", {}, 
            [this](const nlohmann::json& params) {
                auto entity = worldRepository->createEntity();
                std::stringstream ss;
                ss << "Created entity with ID: " << entt::to_integral(entity);
                return ss.str();
            });

        // Tool: delete_entity
        registerTool("delete_entity", "Deletes an entity from the world", 
            {{"id", "The ID of the entity to delete", ToolParamType::NUMBER}}, 
            [this](const nlohmann::json& params) {
                if (!params.contains("id")) return std::string("Error: Missing 'id' parameter");
                auto id = params.at("id").get<uint32_t>();
                entt::entity entity = static_cast<entt::entity>(id);
                worldRepository->deleteEntities({entity});
                return std::string("Deleted entity");
            });

        // Tool: list_component_types
        registerTool("list_component_types", "Lists all available component types", {}, 
            [](const nlohmann::json& params) {
                std::string result = "Available component types:\n";
                for (const auto& def : ComponentTypes::getComponents()) {
                    result += "- " + def.name + " (ID: " + std::to_string(def.type) + ")\n";
                }
                return result;
            });

        // Tool: add_component
        registerTool("add_component", "Adds a component to an entity", 
            {
                {"entity_id", "The ID of the entity", ToolParamType::NUMBER},
                {"component_type_id", "The ID of the component type", ToolParamType::NUMBER}
            }, 
            [this](const nlohmann::json& params) {
                if (!params.contains("entity_id") || !params.contains("component_type_id"))
                    return std::string("Error: Missing 'entity_id' or 'component_type_id' parameter");
                
                auto entityId = params.at("entity_id").get<uint32_t>();
                auto componentTypeId = params.at("component_type_id").get<int>();
                
                entt::entity entity = static_cast<entt::entity>(entityId);
                ComponentType type = static_cast<ComponentType>(componentTypeId);
                
                worldRepository->createComponent(entity, type);
                return std::string("Component added successfully");
            });

        // Tool: get_entity_info
        registerTool("get_entity_info", "Returns information about an entity", 
            {{"id", "The ID of the entity", ToolParamType::NUMBER}}, 
            [this](const nlohmann::json& params) {
                if (!params.contains("id")) return std::string("Error: Missing 'id' parameter");
                
                auto id = params.at("id").get<uint32_t>();
                entt::entity entity = static_cast<entt::entity>(id);
                
                auto* metadata = worldRepository->getEntity(entity);
                if (!metadata) return std::string("Error: Entity not found or has no metadata");
                
                std::string result = "Entity Info:\n";
                result += "Name: " + metadata->name + "\n";
                result += "Components: ";
                
                for (const auto& def : ComponentTypes::getComponents()) {
                    if (def.hasComponent(*worldRepository, entity)) {
                        result += def.name + ", ";
                    }
                }
                return result;
            });
    }
}
