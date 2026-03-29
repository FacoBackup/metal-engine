#include "ComponentType.h"

#include "../../common/Icons.h"
#include "common/Reflection.h"
#include "../repository/WorldRepository.h"
#include "../../ApplicationContext.h"
#include "../dto/TransformComponent.h"
#include "../dto/StaticGeometryComponent.h"
#include "../dto/InstancedGeometryComponent.h"
#include "../dto/AnimatedGeometryComponent.h"
#include "../dto/MetadataComponent.h"
#include "../dto/ScriptComponent.h"
#include "engine/dto/LightComponent.h"
#include "../dto/RigidBodyComponent.h"
#include "../dto/BoxColliderComponent.h"
#include "../dto/SphereColliderComponent.h"
#include "../dto/CapsuleColliderComponent.h"
#include "../dto/MeshColliderComponent.h"


#define DEFINE_COMPONENT(TYPE, NAME, JSON_KEY, ICON, DEPS, CLASS) \
{ \
TYPE, NAME, JSON_KEY, ICON, DEPS, \
[](WorldRepository &repo, entt::entity entityId) { \
auto &comp = repo.registry.emplace_or_replace<CLASS>(entityId); \
comp.setEntityId(entityId); \
}, \
[](WorldRepository &repo, entt::entity entityId) { \
if (auto *comp = repo.registry.try_get<CLASS>(entityId)) { \
return comp->toJson(); \
} \
return nlohmann::json(); \
}, \
[](WorldRepository &repo, entt::entity entityId, const nlohmann::json &j) { \
repo.registry.get<CLASS>(entityId).fromJson(j); \
}, \
[](WorldRepository &repo, entt::entity entityId) -> Reflection* { \
if (auto *comp = repo.registry.try_get<CLASS>(entityId)) { \
return static_cast<Reflection *>(comp); \
} \
return nullptr; \
}, \
[](const WorldRepository &repo, entt::entity entityId) { \
return repo.registry.all_of<CLASS>(entityId); \
} \
}

namespace Metal::ComponentTypes {
    const std::vector<ComponentDefinition> &getComponents() {
        static const std::vector<ComponentDefinition> COMPONENTS = {
#define X(TYPE, NAME, JSON_KEY, ICON, DEPS, CLASS) DEFINE_COMPONENT(TYPE, NAME, JSON_KEY, ICON, DEPS, CLASS),
            COMPONENTS_LIST
#undef X
        };
        return COMPONENTS;
    }

    const char *NameOf(ComponentType mode) {
        for (const auto &comp: getComponents()) {
            if (comp.type == mode) return comp.name.c_str();
        }
        return nullptr;
    }

    const char *IconOf(ComponentType mode) {
        for (const auto &comp: getComponents()) {
            if (comp.type == mode) return comp.icon.c_str();
        }
        return nullptr;
    }
}
