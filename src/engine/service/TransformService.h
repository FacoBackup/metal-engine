#ifndef TRANSFORMSERVICE_H
#define TRANSFORMSERVICE_H
#include <string>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "../../common/ISync.h"
#include "../../common/IService.h"

namespace Metal {
    struct TransformComponent;

    struct WorldRepository;
    class RayTracingService;

    class TransformService final : public IService, public ISync {
        WorldRepository *worldRepository = nullptr;
        RayTracingService *rayTracingService = nullptr;

        glm::vec3 distanceAux{};
        glm::mat4x4 auxMat4{};
        glm::vec3 translation{};
        glm::mat4x4 auxMat42{};

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"WorldRepository", worldRepository},
                {"RayTracingService", rayTracingService}
            };
        }

        void onSync() override;

        void transform(TransformComponent *st, const TransformComponent *parentTransform);
    };
} // Metal

#endif
