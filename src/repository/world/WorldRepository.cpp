#include "WorldRepository.h"

#include "../../context/ApplicationContext.h"
#include "../../enum/ComponentType.h"

namespace Metal {
    WorldRepository::WorldRepository(): AbstractRuntimeComponent() {
        const auto root = registry.create(static_cast<entt::entity>(ROOT_ID));
        auto &entity = registry.emplace<EntityComponent>(root);
        entity.initialize(true);
        entity.name = "Scene";
        registry.emplace<HierarchyComponent>(root);
    }

    EntityID WorldRepository::createEntity(std::string name, const bool container) {
        registerChange();

        const auto entity = registry.create();
        auto &entityComp = registry.emplace<EntityComponent>(entity);
        entityComp.initialize(container);
        entityComp.name = std::move(name);

        registry.emplace<HierarchyComponent>(entity);
        linkEntities(ROOT_ID, static_cast<EntityID>(entity));

        return static_cast<EntityID>(entity);
    }

    void WorldRepository::linkEntities(EntityID parentId, EntityID childId) {
        registerChange();

        const auto child = static_cast<entt::entity>(childId);
        auto &childHierarchy = registry.get<HierarchyComponent>(child);

        if (childHierarchy.parent != EMPTY_ENTITY) {
            const auto oldParent = static_cast<entt::entity>(childHierarchy.parent);
            if (registry.valid(oldParent)) {
                auto &oldParentHierarchy = registry.get<HierarchyComponent>(oldParent);
                oldParentHierarchy.children.erase(
                    std::ranges::remove(oldParentHierarchy.children, childId).begin(),
                    oldParentHierarchy.children.end());
            }
        }

        const auto newParent = static_cast<entt::entity>(parentId);
        if (registry.valid(newParent)) {
            auto &newParentHierarchy = registry.get<HierarchyComponent>(newParent);
            newParentHierarchy.children.push_back(childId);
            childHierarchy.parent = parentId;
        }
    }

    EntityComponent *WorldRepository::getEntity(const EntityID node) {
        const auto entity = static_cast<entt::entity>(node);
        if (registry.valid(entity)) {
            return &registry.get<EntityComponent>(entity);
        }
        return nullptr;
    }

    void WorldRepository::deleteRecursively(const std::vector<EntityID> &entities) {
        for (EntityID entityId: entities) {
            const auto entity = static_cast<entt::entity>(entityId);
            if (!registry.valid(entity)) continue;

            if (hiddenEntities.contains(entityId)) {
                hiddenEntities.erase(entityId);
            }
            if (culled.contains(entityId)) {
                culled.erase(entityId);
            }

            if (registry.all_of<HierarchyComponent>(entity)) {
                auto &hierarchy = registry.get<HierarchyComponent>(entity);
                if (!hierarchy.children.empty()) {
                    std::vector<EntityID> childrenToDelete = hierarchy.children;
                    deleteRecursively(childrenToDelete);
                }

                auto parentId = hierarchy.parent;
                const auto parent = static_cast<entt::entity>(parentId);
                if (registry.valid(parent) && registry.all_of<HierarchyComponent>(parent)) {
                    auto &parentHierarchy = registry.get<HierarchyComponent>(parent);
                    parentHierarchy.children.erase(
                        std::ranges::remove(parentHierarchy.children, entityId).begin(),
                        parentHierarchy.children.end());
                }
            }
            registry.destroy(entity);
        }
    }

    void WorldRepository::deleteEntities(const std::vector<EntityID> &entities) {
        registerChange();
        deleteRecursively(entities);
        CTX.engineContext.setUpdateLights(true);
        CTX.engineContext.setUpdateVolumes(true);
        CTX.rayTracingService.markDirty();
    }

    void WorldRepository::changeVisibility(EntityID entity, bool isVisible) {
        registerChange();
        changeVisibilityRecursively(entity, isVisible);
        CTX.engineContext.setUpdateLights(true);
        CTX.engineContext.setUpdateVolumes(true);
        CTX.rayTracingService.markDirty();
    }

    void WorldRepository::changeVisibilityRecursively(EntityID entity, const bool isVisible) {
        if (isVisible) {
            hiddenEntities.erase(entity);
        } else {
            hiddenEntities.insert({entity, true});
        }

        const auto e = static_cast<entt::entity>(entity);
        if (registry.valid(e) && registry.all_of<HierarchyComponent>(e)) {
            for (const auto child: registry.get<HierarchyComponent>(e).children) {
                changeVisibilityRecursively(child, isVisible);
            }
        }
    }

    void WorldRepository::createComponent(const EntityID entityId, ComponentTypes::ComponentType type) {
        const auto entity = static_cast<entt::entity>(entityId);
        if (!registry.valid(entity)) {
            return;
        }
        switch (type) {
            case ComponentTypes::PRIMITIVE: {
                auto &mesh = registry.emplace_or_replace<PrimitiveComponent>(entity);
                mesh.setEntityId(entityId);
                createComponent(entityId, ComponentTypes::TRANSFORM);
                CTX.rayTracingService.markDirty();
                break;
            }
            case ComponentTypes::SPHERE_LIGHT: {
                auto light = std::make_unique<SphereLightComponent>();
                light->setEntityId(entityId);
                registry.emplace_or_replace<std::unique_ptr<LightComponent>>(entity, std::move(light));
                createComponent(entityId, ComponentTypes::TRANSFORM);
                CTX.engineContext.setUpdateLights(true);
                break;
            }
            case ComponentTypes::PLANE_LIGHT: {
                auto light = std::make_unique<PlaneLightComponent>();
                light->setEntityId(entityId);
                registry.emplace_or_replace<std::unique_ptr<LightComponent>>(entity, std::move(light));
                createComponent(entityId, ComponentTypes::TRANSFORM);
                CTX.engineContext.setUpdateLights(true);
                break;
            }
            case ComponentTypes::VOLUME: {
                auto &vol = registry.emplace_or_replace<VolumeComponent>(entity);
                vol.setEntityId(entityId);
                createComponent(entityId, ComponentTypes::TRANSFORM);
                CTX.engineContext.setUpdateVolumes(true);
                break;
            }
            case ComponentTypes::TRANSFORM: {
                if (!registry.all_of<TransformComponent>(entity)) {
                    auto &trans = registry.emplace<TransformComponent>(entity);
                    trans.setEntityId(entityId);
                    CTX.worldGridRepository.getCurrentTile()->entities.push_back(entityId);
                    registry.get<EntityComponent>(entity).onTile = CTX.worldGridRepository.getCurrentTile()->id;
                }
                break;
            }
            case ComponentTypes::ATMOSPHERE: {
                auto &atmo = registry.emplace_or_replace<AtmosphereComponent>(entity);
                atmo.setEntityId(entityId);
                CTX.engineContext.setGISettingsUpdated(true);
                break;
            }
            default:
                break;
        }
    }

    nlohmann::json WorldRepository::toJson() const {
        nlohmann::json j;
        j["camera"] = camera.toJson();

        nlohmann::json entitiesJson;
        for (auto entity : registry.view<entt::entity>()) {
            const auto id = static_cast<EntityID>(entity);
            nlohmann::json ej;

            if (registry.all_of<EntityComponent>(entity)) {
                ej["entity"] = registry.get<EntityComponent>(entity).toJson();
            }
            if (registry.all_of<HierarchyComponent>(entity)) {
                auto &h = registry.get<HierarchyComponent>(entity);
                ej["hierarchy"] = {{"parent", h.parent}, {"children", h.children}};
            }
            if (registry.all_of<PrimitiveComponent>(entity)) {
                ej["mesh"] = registry.get<PrimitiveComponent>(entity).toJson();
            }
            if (registry.all_of<TransformComponent>(entity)) {
                ej["transform"] = registry.get<TransformComponent>(entity).toJson();
            }
            if (registry.all_of<std::unique_ptr<LightComponent>>(entity)) {
                ej["light"] = registry.get<std::unique_ptr<LightComponent>>(entity)->toJson();
            }
            if (registry.all_of<VolumeComponent>(entity)) {
                ej["volume"] = registry.get<VolumeComponent>(entity).toJson();
            }
            if (registry.all_of<AtmosphereComponent>(entity)) {
                ej["atmosphere"] = registry.get<AtmosphereComponent>(entity).toJson();
            }
            entitiesJson[std::to_string(id)] = ej;
        }
        j["registry"] = entitiesJson;
        j["hiddenEntities"] = hiddenEntities;
        return j;
    }

    void WorldRepository::fromJson(const nlohmann::json &j) {
        if (j.contains("camera")) camera.fromJson(j.at("camera"));

        registry.clear();
        if (j.contains("registry")) {
            for (auto const &[key, val]: j.at("registry").items()) {
                const auto id = static_cast<EntityID>(std::stoull(key));
                const auto entity = registry.create(static_cast<entt::entity>(id));

                if (val.contains("entity")) {
                    registry.emplace<EntityComponent>(entity).fromJson(val.at("entity"));
                }
                if (val.contains("hierarchy")) {
                    auto &h = registry.emplace<HierarchyComponent>(entity);
                    h.parent = val.at("hierarchy").at("parent").get<EntityID>();
                    h.children = val.at("hierarchy").at("children").get<std::vector<EntityID>>();
                }
                if (val.contains("mesh")) {
                    registry.emplace<PrimitiveComponent>(entity).fromJson(val.at("mesh"));
                }
                if (val.contains("transform")) {
                    registry.emplace<TransformComponent>(entity).fromJson(val.at("transform"));
                }
                if (val.contains("light")) {
                    auto &lj = val.at("light");
                    std::string type = lj.at("lightType").get<std::string>();
                    if (type == "SPHERE") {
                        auto light = std::make_unique<SphereLightComponent>();
                        light->fromJson(lj);
                        registry.emplace<std::unique_ptr<LightComponent>>(entity, std::move(light));
                    } else if (type == "PLANE") {
                        auto light = std::make_unique<PlaneLightComponent>();
                        light->fromJson(lj);
                        registry.emplace<std::unique_ptr<LightComponent>>(entity, std::move(light));
                    }
                }
                if (val.contains("volume")) {
                    registry.emplace<VolumeComponent>(entity).fromJson(val.at("volume"));
                }
                if (val.contains("atmosphere")) {
                    registry.emplace<AtmosphereComponent>(entity).fromJson(val.at("atmosphere"));
                }
            }
        }

        if (j.contains("hiddenEntities")) {
            hiddenEntities = j.at("hiddenEntities").get<std::unordered_map<EntityID, bool>>();
        }
    }
} // Metal
