#include "LuaContext.h"
#include <iostream>
#include <entt/entt.hpp>
#include "../repository/WorldRepository.h"
#include "../dto/MetadataComponent.h"
#include "../enum/ComponentType.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../EngineContext.h"
#include "../service/TransformService.h"
#include "../dto/TransformComponent.h"
#include "../../common/LoggerUtil.h"
#include "engine/dto/ScriptComponent.h"

namespace Metal {
    void LuaContext::initialize(WorldRepository* worldRepository, EngineContext* engineContext, TransformService* transformService) {
        lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table, sol::lib::string,
                           sol::lib::debug);
        
        // Redirect print to LOG_INFO
        lua.set_function("print", [](sol::variadic_args args) {
            std::string msg;
            for (auto arg : args) {
                if (!msg.empty()) msg += " ";
                msg += arg.as<std::string>();
            }
            LOG_INFO(msg);
        });

        std::cout << "LuaContext: Lua initialized." << std::endl;

        // Math Bindings
        lua.new_usertype<glm::vec3>("vec3",
            sol::no_constructor,
            "x", &glm::vec3::x,
            "y", &glm::vec3::y,
            "z", &glm::vec3::z,
            sol::meta_function::addition, [](const glm::vec3& a, const glm::vec3& b) { return a + b; },
            sol::meta_function::subtraction, [](const glm::vec3& a, const glm::vec3& b) { return a - b; },
            sol::meta_function::multiplication, sol::overload(
                [](const glm::vec3& a, const glm::vec3& b) { return a * b; },
                [](const glm::vec3& a, float b) { return a * b; },
                [](float a, const glm::vec3& b) { return a * b; }
            ),
            sol::meta_function::division, sol::overload(
                [](const glm::vec3& a, const glm::vec3& b) { return a / b; },
                [](const glm::vec3& a, float b) { return a / b; }
            )
        );

        lua.new_usertype<glm::quat>("quat",
            sol::no_constructor,
            "w", &glm::quat::w,
            "x", &glm::quat::x,
            "y", &glm::quat::y,
            "z", &glm::quat::z,
            sol::meta_function::multiplication, sol::overload(
                [](const glm::quat& a, const glm::quat& b) { return a * b; },
                [](const glm::quat& q, const glm::vec3& v) { return q * v; }
            )
        );

        lua.new_usertype<glm::mat4>("mat4",
            sol::no_constructor,
            sol::meta_function::multiplication, sol::overload(
                [](const glm::mat4& a, const glm::mat4& b) { return a * b; },
                [](const glm::mat4& m, const glm::vec4& v) { return m * v; }
            )
        );

        // Core Engine Bindings
        if (engineContext) {
            lua.new_usertype<EngineContext>("EngineContext",
                sol::no_constructor,
                "currentTimeMs", sol::readonly(&EngineContext::currentTimeMs),
                "deltaTime", sol::readonly(&EngineContext::deltaTime)
            );
            lua["engine"] = engineContext;
        }

        if (transformService) {
            lua.new_usertype<TransformService>("TransformService",
                sol::no_constructor,
                "transform", &TransformService::transform
            );
            lua["transformService"] = transformService;
        }

        lua.new_usertype<TransformComponent>("TransformComponent",
            "translation", &TransformComponent::translation,
            "rotation", &TransformComponent::rotation,
            "scale", &TransformComponent::scale,
            "model", &TransformComponent::model,
            "isStatic", &TransformComponent::isStatic
        );

        lua.new_usertype<ScriptComponent>("ScriptComponent",
            "scriptPath", &ScriptComponent::scriptPath
        );

        lua.new_usertype<MetadataComponent>("MetadataComponent",
            "name", &MetadataComponent::name
        );

        // Bindings
        lua.new_enum("ComponentType",
                     "STATIC_GEOMETRY", STATIC_GEOMETRY,
                     "INSTANCED_GEOMETRY", INSTANCED_GEOMETRY,
                     "ANIMATED_GEOMETRY", ANIMATED_GEOMETRY,
                     "TRANSFORM", TRANSFORM,
                     "METADATA", METADATA,
                     "LIGHT", LIGHT,
                     "SCRIPT", SCRIPT,
                     "RIGID_BODY", RIGID_BODY,
                     "BOX_COLLIDER", BOX_COLLIDER,
                     "SPHERE_COLLIDER", SPHERE_COLLIDER,
                     "CAPSULE_COLLIDER", CAPSULE_COLLIDER,
                     "MESH_COLLIDER", MESH_COLLIDER
        );

        // Treat entt::entity as its underlying type (usually uint32_t) for Lua
        lua.set_function("to_entity", [](uint32_t id) { return static_cast<entt::entity>(id); });
        lua.set_function("from_entity", [](entt::entity id) { return static_cast<uint32_t>(id); });

        lua.new_usertype<WorldRepository>("WorldRepository",
                                          "createEntity", &WorldRepository::createEntity,
                                          "getEntity", &WorldRepository::getEntity,
                                          "hasComponent", &WorldRepository::hasComponent,
                                          "deleteEntities", &WorldRepository::deleteEntities,
                                          "getScript", [](WorldRepository& world, entt::entity entity) -> ScriptComponent* {
                                              return world.registry.try_get<ScriptComponent>(entity);
                                          },
                                          "getTransform", [](WorldRepository& world, entt::entity entity) -> TransformComponent* {
                                              return world.registry.try_get<TransformComponent>(entity);
                                          },
                                          "getMetadata", [](WorldRepository& world, entt::entity entity) -> MetadataComponent* {
                                              return world.registry.try_get<MetadataComponent>(entity);
                                          }
        );

        if (worldRepository) {
            lua["world"] = worldRepository;
        }
    }
} // Metal
