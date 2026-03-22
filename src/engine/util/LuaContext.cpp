#include "LuaContext.h"
#include <iostream>
#include <entt/entt.hpp>
#include "../repository/WorldRepository.h"
#include "../dto/MetadataComponent.h"
#include "../enum/ComponentType.h"

namespace Metal {
    void LuaContext::initialize(WorldRepository* worldRepository) {
        lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table, sol::lib::string,
                           sol::lib::debug);
        std::cout << "LuaContext: Lua initialized." << std::endl;

        // Bindings
        lua.new_enum("ComponentType",
                     "PRIMITIVE", ComponentType::PRIMITIVE,
                     "TRANSFORM", ComponentType::TRANSFORM,
                     "VOLUME", ComponentType::VOLUME,
                     "METADATA", ComponentType::METADATA,
                     "LIGHT", ComponentType::LIGHT,
                     "SCOPED_SCRIPT", ComponentType::SCOPED_SCRIPT,
                     "GLOBAL_SCRIPT", ComponentType::GLOBAL_SCRIPT,
                     "RIGID_BODY", ComponentType::RIGID_BODY,
                     "BOX_COLLIDER", ComponentType::BOX_COLLIDER,
                     "SPHERE_COLLIDER", ComponentType::SPHERE_COLLIDER,
                     "CAPSULE_COLLIDER", ComponentType::CAPSULE_COLLIDER,
                     "MESH_COLLIDER", ComponentType::MESH_COLLIDER
        );

        // Treat entt::entity as its underlying type (usually uint32_t) for Lua
        lua.set_function("to_entity", [](uint32_t id) { return static_cast<entt::entity>(id); });
        lua.set_function("from_entity", [](entt::entity id) { return static_cast<uint32_t>(id); });

        lua.new_usertype<WorldRepository>("WorldRepository",
                                          "createEntity", &WorldRepository::createEntity,
                                          "getEntity", &WorldRepository::getEntity,
                                          "hasComponent", &WorldRepository::hasComponent,
                                          "deleteEntities", &WorldRepository::deleteEntities);

        if (worldRepository) {
            lua["world"] = worldRepository;
        }
    }
} // Metal
