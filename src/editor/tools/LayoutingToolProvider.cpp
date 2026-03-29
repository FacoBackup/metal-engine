#include "LayoutingToolProvider.h"
#include "engine/repository/WorldRepository.h"
#include "editor/service/FilesService.h"
#include "engine/dto/TransformComponent.h"
#include "engine/dto/StaticGeometryComponent.h"
#include "engine/dto/MetadataComponent.h"
#include "engine/service/DirtyStateService.h"
#include "engine/enum/DirtyType.h"
#include "common/FileExtensions.h"
#include <sstream>

namespace Metal {
    void LayoutingToolProvider::registerTools() {
        // Tool: list_mesh_files
        registerTool("list_mesh_files", "Lists all available .mesh files in the project.", {},
                     [this](const nlohmann::json &params) { return listMeshFiles(params); });

        // Tool: get_camera_position
        registerTool("get_camera_position", "Returns the current camera position and orientation.", {},
                     [this](const nlohmann::json &params) { return getCameraPosition(params); });

        // Tool: create_mesh_entity
        registerTool("create_mesh_entity",
                     "Creates a new entity with a mesh and optional transform. If 'position' is not provided, the entity should be created in front of the camera.",
                     {
                         {"mesh_id", "The ID/path of the mesh file", ToolParamType::STRING},
                         {"name", "Optional name for the entity (string)", ToolParamType::STRING},
                         {
                             "position", "Optional array of 3 numbers [x, y, z] - Pass as string: '[x,y,z]'",
                             ToolParamType::STRING
                         },
                         {
                             "rotation_euler",
                             "Optional array of 3 numbers [pitch, yaw, roll] in degrees - Pass as string: '[x,y,z]'",
                             ToolParamType::STRING
                         },
                         {
                             "scale", "Optional array of 3 numbers [sx, sy, sz] - Pass as string: '[x,y,z]'",
                             ToolParamType::STRING
                         }
                     },
                     [this](const nlohmann::json &params) { return createMeshEntity(params); });

        // Tool: set_entity_transform
        registerTool("set_entity_transform", "Updates the transform of an existing entity.",
                     {
                         {"entity_id", "The ID of the entity", ToolParamType::NUMBER},
                         {
                             "position", "Optional array of 3 numbers [x, y, z] - Pass as string: '[x,y,z]'",
                             ToolParamType::STRING
                         },
                         {
                             "rotation_euler",
                             "Optional array of 3 numbers [pitch, yaw, roll] in degrees - Pass as string: '[x,y,z]'",
                             ToolParamType::STRING
                         },
                         {
                             "scale", "Optional array of 3 numbers [sx, sy, sz] - Pass as string: '[x,y,z]'",
                             ToolParamType::STRING
                         }
                     },
                     [this](const nlohmann::json &params) { return setEntityTransform(params); });
    }

    std::string LayoutingToolProvider::listMeshFiles(const nlohmann::json &params) {
        auto meshes = filesService->listFilesWithExtension(Metal::FileExtensions::mesh->extension);
        if (meshes.empty()) return std::string("No .mesh files found.");

        std::stringstream ss;
        ss << "Available mesh files:\n";
        for (const auto &mesh: meshes) {
            ss << "- " << mesh << "\n";
        }
        return ss.str();
    }

    std::string LayoutingToolProvider::getCameraPosition(const nlohmann::json &params) {
        auto &camera = worldRepository->camera;
        nlohmann::json j;
        j["position"] = {camera.position.x, camera.position.y, camera.position.z};
        j["pitch"] = camera.pitch;
        j["yaw"] = camera.yaw;
        return j.dump();
    }

    std::string LayoutingToolProvider::createMeshEntity(const nlohmann::json &params) {
        if (!params.contains("mesh_id")) return std::string("Error: Missing 'mesh_id' parameter");

        auto entity = worldRepository->createEntity();

        // Set name if provided
        if (params.contains("name")) {
            if (auto *metadata = worldRepository->getEntity(entity)) {
                metadata->name = params.at("name").get<std::string>();
            }
        }

        // Add and configure TransformComponent
        worldRepository->createComponent(entity, TRANSFORM);
        auto &transform = worldRepository->registry.get<TransformComponent>(entity);

        if (params.contains("position")) {
            transform.translation = parseVec3(params.at("position").get<std::string>());
        } else {
            // Instantiate in front of the camera if no position is specified
            transform.translation = worldRepository->camera.position + getCameraForward() * 5.0f;
        }
        if (params.contains("rotation_euler")) {
            auto rot = parseVec3(params.at("rotation_euler").get<std::string>());
            transform.rotation = glm::quat(
                glm::vec3(glm::radians(rot.x), glm::radians(rot.y), glm::radians(rot.z)));
        }
        if (params.contains("scale")) {
            transform.scale = parseVec3(params.at("scale").get<std::string>());
        }
        
        // Add and configure StaticGeometryComponent
        worldRepository->createComponent(entity, STATIC_GEOMETRY);
        auto &primitive = worldRepository->registry.get<StaticGeometryComponent>(entity);
        primitive.meshId = params.at("mesh_id").get<std::string>();
        
        dirtyStateService->markEntityDirty(entity, DirtyType::Transform | DirtyType::Material | DirtyType::Mesh);

        std::stringstream ss;
        ss << "Created entity '" << (params.contains("name")
                                         ? params.at("name").get<std::string>()
                                         : "New entity")
                << "' (ID: " << entt::to_integral(entity) << ") with mesh: " << primitive.meshId;
        return ss.str();
    }

    std::string LayoutingToolProvider::setEntityTransform(const nlohmann::json &params) {
        if (!params.contains("entity_id")) return std::string("Error: Missing 'entity_id' parameter");

        entt::entity entity = static_cast<entt::entity>(params.at("entity_id").get<uint32_t>());

        if (!worldRepository->registry.valid(entity)) return std::string("Error: Invalid entity ID");

        if (!worldRepository->registry.all_of<TransformComponent>(entity)) {
            worldRepository->createComponent(entity, TRANSFORM);
        }

        auto &transform = worldRepository->registry.get<TransformComponent>(entity);

        if (params.contains("position")) {
            transform.translation = parseVec3(params.at("position").get<std::string>());
        }
        if (params.contains("rotation_euler")) {
            auto rot = parseVec3(params.at("rotation_euler").get<std::string>());
            transform.rotation = glm::quat(
                glm::vec3(glm::radians(rot.x), glm::radians(rot.y), glm::radians(rot.z)));
        }
        if (params.contains("scale")) {
            transform.scale = parseVec3(params.at("scale").get<std::string>());
        }

        dirtyStateService->markEntityDirty(entity, DirtyType::Transform);

        return std::string("Transform updated successfully for entity ") + std::to_string(
                   entt::to_integral(entity));
    }

    glm::vec3 LayoutingToolProvider::parseVec3(const std::string &s) {
        try {
            auto j = nlohmann::json::parse(s);
            if (j.is_array() && j.size() == 3) {
                return {j[0].get<float>(), j[1].get<float>(), j[2].get<float>()};
            }
        } catch (...) {
        }
        return {0, 0, 0};
    }

    glm::vec3 LayoutingToolProvider::getCameraForward() const {
        auto &camera = worldRepository->camera;
        return glm::normalize(glm::vec3(
            -std::sin(camera.yaw) * std::cos(camera.pitch),
            std::sin(camera.pitch),
            -std::cos(camera.yaw) * std::cos(camera.pitch)
        ));
    }
}
