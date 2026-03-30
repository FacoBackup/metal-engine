#ifndef WORLDREPOSITORY_H
#define WORLDREPOSITORY_H

#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include <common/IRepository.h>
#include <engine/enum/ComponentType.h>
#include <common/ILoader.h>

namespace Metal {
    struct TransformComponent;
    struct MetadataComponent;
    class Reflection;
    class DirectoryService;
    class HistoryService;
    class DirtyStateService;

    struct EntityState {
        entt::entity id;
        nlohmann::json data;
    };

    struct WorldRepository final : IRepository, ILoader {
        DirectoryService *directoryService = nullptr;
        HistoryService *historyService = nullptr;
        DirtyStateService *dirtyStateService = nullptr;

        entt::registry registry{};
        std::unordered_map<entt::entity, bool> culled{};
        std::unordered_map<entt::entity, bool> hiddenEntities{};

        std::vector<Dependency> getDependencies() override {
            return {
                {"DirectoryService", &directoryService},
                {"HistoryService", &historyService},
                {"DirtyStateService", &dirtyStateService}
            };
        }

        void createComponent(entt::entity entity, ComponentType type);

        entt::entity createEntity();

        [[nodiscard]] MetadataComponent *getEntity(entt::entity node);

        [[nodiscard]] bool hasComponent(entt::entity entity, ComponentType type) const;

        EntityState serializeEntityComplete(std::vector<entt::entity>::value_type entityId);

        void deleteEntities(const std::vector<entt::entity> &entities);

        void changeVisibility(entt::entity entity, bool isVisible);

        void clear() override;

        void registerFields() override;

        void load(const std::string &absolutePath) override;

        bool isCompatible(const std::string &absolutePath) override;

        void deserializeEntityComplete(const EntityState &state);
    };
} // Metal

#endif //WORLDREPOSITORY_H
