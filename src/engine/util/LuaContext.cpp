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
                     "PRIMITIVE", PRIMITIVE,
                     "TRANSFORM", TRANSFORM,
                     "VOLUME", VOLUME,
                     "METADATA", METADATA,
                     "LIGHT", LIGHT,
                     "SCOPED_SCRIPT", SCOPED_SCRIPT,
                     "GLOBAL_SCRIPT", GLOBAL_SCRIPT,
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
                                          "deleteEntities", &WorldRepository::deleteEntities);

        if (worldRepository) {
            lua["world"] = worldRepository;
        }
    }
} // Metal
