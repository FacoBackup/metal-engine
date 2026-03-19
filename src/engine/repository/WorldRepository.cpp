#include "WorldRepository.h"
#include "../service/RayTracingService.h"
#include "../../editor/service/HistoryService.h"

#include "../enum/ComponentType.h"
#include "../../editor/dto/SceneData.h"
#include "../../common/serialization-definitions.h"
#include "../../core/DirectoryService.h"

namespace Metal {
    entt::entity WorldRepository::createEntity() {
        registerChange();

        const auto entity = registry.create();
        createComponent(entity, METADATA);

        if (historyService) {
            auto createdState = std::make_shared<EntityState>();
            historyService->recordAction(
                [this, entity, createdState]() {
                    // Undo: Delete entity
                    if (registry.valid(entity)) {
                        auto serialized = serializeEntityComplete(entity);
                        createdState->data = serialized.data;
                        createdState->id = serialized.id;
                        registry.destroy(entity);
                        if (hiddenEntities.contains(entity)) hiddenEntities.erase(entity);
                        if (culled.contains(entity)) culled.erase(entity);
                        rayTracingService->markDirty();
                    }
                },
                [this, createdState]() {
                    deserializeEntityComplete(*createdState);
                    rayTracingService->markDirty();
                }
            );
        }

        return entity;
    }

    MetadataComponent *WorldRepository::getEntity(const entt::entity node) {
        if (registry.valid(node)) {
            return &registry.get<MetadataComponent>(node);
        }
        return nullptr;
    }

    EntityState WorldRepository::serializeEntityComplete(std::vector<entt::entity>::value_type entityId) {
        nlohmann::json ej;
        if (registry.all_of<MetadataComponent>(entityId)) {
            ej["entity"] = registry.get<MetadataComponent>(entityId).toJson();
        }
        for (const auto &compDef: ComponentTypes::getComponents()) {
            auto cj = compDef.toJson(*this, entityId);
            if (!cj.is_null()) {
                ej[compDef.jsonKey] = std::move(cj);
            }
        }
        EntityState state = {entityId, ej};
        return state;
    }

    void WorldRepository::deleteEntities(const std::vector<entt::entity> &entities) {
        registerChange();

        if (historyService) {
            std::vector<EntityState> deletedStates;
            for (auto entityId: entities) {
                if (registry.valid(entityId)) {
                    deletedStates.push_back(serializeEntityComplete(entityId));
                }
            }

            historyService->recordAction(
                [this, deletedStates]() {
                    // Undo: Restore entities
                    for (const EntityState &state: deletedStates) {
                        deserializeEntityComplete(state);
                    }
                    rayTracingService->markDirty();
                },
                [this, entities]() {
                    // Redo: Delete entities
                    for (entt::entity entityId: entities) {
                        if (registry.valid(entityId)) {
                            if (hiddenEntities.contains(entityId)) hiddenEntities.erase(entityId);
                            if (culled.contains(entityId)) culled.erase(entityId);
                            registry.destroy(entityId);
                        }
                    }
                    rayTracingService->markDirty();
                }
            );
        }

        for (entt::entity entityId: entities) {
            if (!registry.valid(entityId)) continue;

            if (hiddenEntities.contains(entityId)) {
                hiddenEntities.erase(entityId);
            }
            if (culled.contains(entityId)) {
                culled.erase(entityId);
            }

            registry.destroy(entityId);
        }
        rayTracingService->markDirty();
    }

    void WorldRepository::changeVisibility(entt::entity entity, bool isVisible) {
        registerChange();
        bool wasVisible = !hiddenEntities.contains(entity);
        if (isVisible == wasVisible) return;

        if (historyService) {
            historyService->recordAction(
                [this, entity, wasVisible]() {
                    this->changeVisibility(entity, wasVisible);
                },
                [this, entity, isVisible]() {
                    this->changeVisibility(entity, isVisible);
                }
            );
        }

        if (isVisible) {
            hiddenEntities.erase(entity);
        } else {
            hiddenEntities.insert({entity, true});
        }
        rayTracingService->markDirty();
    }

    void WorldRepository::loadScene(const std::string &sceneId) {
        SceneData sceneData;
        const auto pathToFile = directoryService->getAssetDirectory() + FORMAT_FILE_SCENE(sceneId);
        PARSE_TEMPLATE(sceneData, pathToFile)

        for (auto &entityData: sceneData.entities) {
            const auto entityId = createEntity();

            auto &entityComp = registry.get<MetadataComponent>(entityId);
            entityComp.name = entityData.entity.name;

            createComponent(entityId, TRANSFORM);
            auto &transform = registry.get<TransformComponent>(entityId);
            transform.translation = entityData.transform.translation;
            transform.rotationEuler = entityData.transform.rotationEuler;
            transform.scale = entityData.transform.scale;
            transform.gizmoCenter = entityData.transform.gizmoCenter;
            transform.isStatic = entityData.transform.isStatic;

            if (entityData.primitive) {

                createComponent(entityId, PRIMITIVE);
                auto &primitive = registry.get<PrimitiveComponent>(entityId);
                primitive.meshId = entityData.primitive->meshId;
                primitive.albedo = entityData.primitive->albedo;
                primitive.roughness = entityData.primitive->roughness;
                primitive.metallic = entityData.primitive->metallic;
                primitive.albedoColor = entityData.primitive->albedoColor;
                primitive.roughnessFactor = entityData.primitive->roughnessFactor;
                primitive.metallicFactor = entityData.primitive->metallicFactor;
                primitive.transmissionFactor = entityData.primitive->transmissionFactor;
                primitive.thicknessFactor = entityData.primitive->thicknessFactor;
                primitive.ior = entityData.primitive->ior;
            }
        }
    }

    void WorldRepository::createComponent(const entt::entity entityId, ComponentType type) {
        if (!registry.valid(entityId)) {
            return;
        }

        for (const auto &compDef: ComponentTypes::getComponents()) {
            if (compDef.type == type) {
                for (const auto &dep: compDef.dependencies) {
                    createComponent(entityId, dep);
                }
                if (compDef.creator) {
                    compDef.creator(*this, entityId);
                }
                break;
            }
        }
    }

    bool WorldRepository::hasComponent(entt::entity entity, ComponentType type) const {
        if (!registry.valid(entity)) return false;

        for (const auto &compDef: ComponentTypes::getComponents()) {
            if (compDef.type == type) {
                return compDef.hasComponent(*this, entity);
            }
        }
        return false;
    }

    nlohmann::json WorldRepository::toJson() const {
        nlohmann::json j;
        j["camera"] = camera.toJson();

        nlohmann::json entitiesJson;
        for (auto entity: registry.view<entt::entity>()) {
            nlohmann::json ej;

            if (registry.all_of<MetadataComponent>(entity)) {
                ej["entity"] = registry.get<MetadataComponent>(entity).toJson();
            }

            for (const auto &compDef: ComponentTypes::getComponents()) {
                auto cj = compDef.toJson(const_cast<WorldRepository &>(*this), entity);
                if (!cj.is_null()) {
                    ej[compDef.jsonKey] = std::move(cj);
                }
            }
            entitiesJson[std::to_string(entt::to_integral(entity))] = ej;
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
                const auto id = static_cast<entt::entity>(static_cast<uint32_t>(std::stoul(key)));
                const auto entity = registry.create(id);

                if (val.contains("entity")) {
                    registry.emplace<MetadataComponent>(entity).fromJson(val.at("entity"));
                }

                for (const auto &compDef: ComponentTypes::getComponents()) {
                    if (val.contains(compDef.jsonKey)) {
                        compDef.creator(*this, id);
                        compDef.fromJson(*this, id, val.at(compDef.jsonKey));
                    }
                }
            }
        }

        if (j.contains("hiddenEntities")) {
            hiddenEntities = j.at("hiddenEntities").get<std::unordered_map<entt::entity, bool> >();
        }
    }

    void WorldRepository::deserializeEntityComplete(const EntityState &state) {
        if (!registry.valid(state.id)) {
            const auto entity = registry.create(state.id);
            if (state.data.contains("entity")) {
                registry.emplace<MetadataComponent>(entity).fromJson(state.data.at("entity"));
            }
            for (const auto &compDef: ComponentTypes::getComponents()) {
                if (state.data.contains(compDef.jsonKey)) {
                    compDef.creator(*this, state.id);
                    compDef.fromJson(*this, state.id, state.data.at(compDef.jsonKey));
                }
            }
        }
    }
} // Metal
