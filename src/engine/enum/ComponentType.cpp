#include "ComponentType.h"

#include "../../common/Icons.h"
#include "../EngineContext.h"
#include "../repository/WorldRepository.h"
#include "../service/RayTracingService.h"
#include "../../ApplicationContext.h"
#include "../dto/TransformComponent.h"
#include "../dto/PrimitiveComponent.h"
#include "../dto/VolumeComponent.h"
#include "../dto/MetadataComponent.h"
#include "engine/dto/LightComponent.h"

#define DEFINE_COMPONENT(TYPE, NAME, JSON_KEY, ICON, DEPS, CLASS, CREATOR) \
{ \
TYPE, NAME, JSON_KEY, ICON, DEPS, \
CREATOR, \
[](WorldRepository &repo, entt::entity entityId) { \
if (auto *comp = repo.registry.try_get<CLASS>(entityId)) { \
return comp->toJson(); \
} \
return nlohmann::json(); \
}, \
[](WorldRepository &repo, entt::entity entityId, const nlohmann::json &j) { \
repo.registry.get<CLASS>(entityId).fromJson(j); \
}, \
[](WorldRepository &repo, entt::entity entityId) -> Inspectable* { \
if (auto *comp = repo.registry.try_get<CLASS>(entityId)) { \
return static_cast<Inspectable *>(comp); \
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
            DEFINE_COMPONENT(
                PRIMITIVE, "Primitive", "mesh", Icons::view_in_ar, {TRANSFORM}, PrimitiveComponent,
                [](WorldRepository &repo, entt::entity entityId) {
                auto &mesh = repo.registry.emplace_or_replace<PrimitiveComponent>(entityId);
                mesh.setEntityId(entityId);
                }
            ),
            DEFINE_COMPONENT(
                TRANSFORM, "Transformation", "transform", Icons::transform, {}, TransformComponent,
                [](WorldRepository &repo, entt::entity entityId) {
                if (!repo.registry.all_of<TransformComponent>(entityId)) {
                auto &trans = repo.registry.emplace<TransformComponent>(entityId);
                trans.setEntityId(entityId);
                }
                }
            ),
            DEFINE_COMPONENT(
                VOLUME, "Volume Component", "volume", Icons::blur_on, {TRANSFORM}, VolumeComponent,
                [](WorldRepository &repo, entt::entity entityId) {
                auto &vol = repo.registry.emplace_or_replace<VolumeComponent>(entityId);
                vol.setEntityId(entityId);
                }
            ),
            DEFINE_COMPONENT(
                LIGHT, "Light Component", "light", Icons::lightbulb, std::vector({TRANSFORM, PRIMITIVE}),
                LightComponent,
                [](WorldRepository &repo, entt::entity entityId) {
                auto &vol = repo.registry.emplace_or_replace<LightComponent>(entityId);
                vol.setEntityId(entityId);
                }
            ),
            DEFINE_COMPONENT(
                METADATA, "Metadata", "metadata", Icons::data_array, {}, MetadataComponent,
                [](WorldRepository &repo, entt::entity entityId) {
                auto &atmo = repo.registry.emplace_or_replace<MetadataComponent>(entityId);
                atmo.setEntityId(entityId);
                }
            )
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
