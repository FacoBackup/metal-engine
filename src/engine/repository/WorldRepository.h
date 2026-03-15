#ifndef WORLDREPOSITORY_H
#define WORLDREPOSITORY_H

#include <vector>
#include <glm/ext/scalar_constants.hpp>

#include "../../common/IRepository.h"
#include "../dto/Camera.h"
#include "../enum/ComponentType.h"

namespace Metal {
    struct MetadataComponent;
    class Inspectable;
    class RayTracingService;
    class DirectoryService;
    class HistoryService;

    struct WorldRepository final : IRepository {
        RayTracingService *rayTracingService = nullptr;
        DirectoryService *directoryService = nullptr;
        HistoryService *historyService = nullptr;

        Camera camera{-(glm::pi<float>() / 4), glm::pi<float>() / 4, {10, 10, 10}};
        entt::registry registry{};
        std::unordered_map<entt::entity, bool> culled{};
        std::unordered_map<entt::entity, bool> hiddenEntities{};

        std::vector<Dependency> getDependencies() override {
            return {
                {"RayTracingService", &rayTracingService},
                {"DirectoryService", &directoryService},
                {"HistoryService", &historyService}
            };
        }

        void createComponent(entt::entity entity, ComponentType type);

        entt::entity createEntity();

        [[nodiscard]] MetadataComponent *getEntity(entt::entity node);
        [[nodiscard]] bool hasComponent(entt::entity entity, ComponentType type) const;

        void deleteEntities(const std::vector<entt::entity> &entities);

        void changeVisibility(entt::entity entity, bool isVisible);

        void loadScene(const std::string &sceneId);

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json &j) override;
    };
} // Metal

#endif //WORLDREPOSITORY_H
