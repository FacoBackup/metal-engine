#ifndef TRANSFORMSERVICE_H
#define TRANSFORMSERVICE_H
#include <string>
#include <set>
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <entt/entt.hpp>

#include <common/IAsyncSync.h>
#include <common/IService.h>
#include <common/IEventMember.h>
#include <common/IInit.h>

namespace Metal {
    struct TransformComponent;

    struct WorldRepository;
    class RayTracingService;

    class TransformService final : public IService, public IAsyncSync, public IEventMember, public IInit {
        WorldRepository *worldRepository = nullptr;
        RayTracingService *rayTracingService = nullptr;

        glm::vec3 distanceAux{};
        glm::mat4x4 auxMat4{};
        glm::vec3 translation{};
        glm::mat4x4 auxMat42{};

        std::set<entt::entity> dirtyEntities;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"WorldRepository", &worldRepository},
                {"RayTracingService", &rayTracingService}
            };
        }

        void onInitialize() override;

        std::string getSyncThreadId() const override { return "physics_transform"; }

        void onAsyncSync() override;

        void transform(TransformComponent *st, const TransformComponent *parentTransform);
    };
} // Metal

#endif
