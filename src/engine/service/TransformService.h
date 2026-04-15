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

#include "common/ISync.h"

namespace Metal {
    struct TransformComponent;

    struct WorldRepository;
    class DirtyStateService;
    class QuadTreeService;

    class TransformService final : public IService, public ISync, public IEventMember, public IInit {
        WorldRepository *worldRepository = nullptr;
        DirtyStateService *dirtyStateService = nullptr;
        QuadTreeService *quadTreeService = nullptr;

        glm::vec3 distanceAux{};
        glm::mat4x4 auxMat4{};
        glm::vec3 translation{};
        glm::mat4x4 auxMat42{};

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"WorldRepository", &worldRepository},
                {"DirtyStateService", &dirtyStateService},
                {"QuadTreeService", &quadTreeService}
            };
        }

        void onInitialize() override;

        void onSync() override;

        void transform(TransformComponent *st, const TransformComponent *parentTransform);
    };
} // Metal

#endif
