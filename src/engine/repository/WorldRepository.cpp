#include "WorldRepository.h"
#include "../service/RayTracingService.h"
#include "editor/service/HistoryService.h"
#include "../../ApplicationEventContext.h"

#include "../enum/ComponentType.h"
#include "editor/dto/SceneData.h"
#include "../../common/serialization-definitions.h"
#include "../../core/DirectoryService.h"
#include "common/FileExtensions.h"
#include "editor/dto/FieldModificationEvent.h"

namespace Metal {
    entt::entity WorldRepository::createEntity() {
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
                        ApplicationEventContext::dispatch("BVHNeedsUpdate");
                    }
                },
                [this, createdState]() {
                    deserializeEntityComplete(*createdState);
                    ApplicationEventContext::dispatch("BVHNeedsUpdate");
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
                    ApplicationEventContext::dispatch("BVHNeedsUpdate");
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
                    ApplicationEventContext::dispatch("BVHNeedsUpdate");
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
        ApplicationEventContext::dispatch("BVHNeedsUpdate");
    }

    void WorldRepository::changeVisibility(entt::entity entity, bool isVisible) {
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
        ApplicationEventContext::dispatch("BVHNeedsUpdate");
    }

    void WorldRepository::load(const std::string &absolutePath) {
        if (historyService) {
            historyService->startTransaction("Load Scene " + absolutePath);
        }

        SceneData sceneData;
        PARSE_TEMPLATE(sceneData, absolutePath)

        for (auto &entityData: sceneData.entities) {
            const auto entityId = createEntity();

            auto &entityComp = registry.get<MetadataComponent>(entityId);
            entityComp.name = entityData.entity.name;

            createComponent(entityId, TRANSFORM);
            auto &transform = registry.get<TransformComponent>(entityId);
            transform.translation = entityData.transform.translation;
            transform.rotation = entityData.transform.rotation;
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
                primitive.transmissionFactor = entityData.primitive->transmissionFactor;
                primitive.thicknessFactor = entityData.primitive->thicknessFactor;
                primitive.ior = entityData.primitive->ior;
            }
        }

        if (historyService) {
            historyService->endTransaction();
        }
    }

    bool WorldRepository::isCompatible(const std::string &absolutePath) {
        return absolutePath.ends_with(EXT_SCENE);
    }

    void WorldRepository::createComponent(const entt::entity entityId, ComponentType type) {
        if (!registry.valid(entityId)) {
            return;
        }

        for (const auto &compDef: ComponentTypes::getComponents()) {
            if (compDef.type == type && !hasComponent(entityId, type)) {
                for (const auto &dep: compDef.dependencies) {
                    createComponent(entityId, dep);
                }
                compDef.creator(*this, entityId);
                if (auto *inspectable = compDef.getInspectable(*this, entityId)) {
                    std::string className = inspectable->getClassName();
                    ApplicationEventContext::dispatch(
                        className, std::make_shared<InspectableEventPayload>(inspectable));
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
            registry.create(state.id);
            for (const auto &compDef: ComponentTypes::getComponents()) {
                if (state.data.contains(compDef.jsonKey)) {
                    compDef.creator(*this, state.id);
                    compDef.fromJson(*this, state.id, state.data.at(compDef.jsonKey));
                    if (auto *inspectable = compDef.getInspectable(*this, state.id)) {
                        std::string className = inspectable->getClassName();
                        ApplicationEventContext::dispatch(
                            className, std::make_shared<InspectableEventPayload>(inspectable));
                    }
                }
            }
        }
    }
} // Metal
