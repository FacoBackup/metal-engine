#ifndef METAL_ENGINE_PHYSICSSERVICE_H
#define METAL_ENGINE_PHYSICSSERVICE_H

#include "common/IService.h"
#include "common/IInit.h"
#include "common/IDisposable.h"
#include "common/IEventMember.h"
#include "common/IAsyncSync.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Body/BodyInterface.h>

#include <vector>
#include <unordered_map>
#include <entt/entt.hpp>

namespace Metal {
    struct WorldRepository;
    struct EngineRepository;

    // Layer constants
    namespace Layers {
        static constexpr JPH::ObjectLayer NON_MOVING = 0;
        static constexpr JPH::ObjectLayer MOVING = 1;
        static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
    }

    namespace BroadPhaseLayers {
        static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
        static constexpr JPH::BroadPhaseLayer MOVING(1);
        static constexpr uint32_t NUM_LAYERS = 2;
    }

    class PhysicsService final : public IService, public IInit, public IDisposable, public IEventMember, public IAsyncSync {
        WorldRepository *worldRepository = nullptr;
        EngineRepository *engineRepository = nullptr;

        JPH::PhysicsSystem *physicsSystem = nullptr;
        JPH::TempAllocator *tempAllocator = nullptr;
        JPH::JobSystemThreadPool *jobSystem = nullptr;

        bool enabled = false;
        std::unordered_map<entt::entity, JPH::BodyID> entityToBody;

    public:
        PhysicsService() = default;

        ~PhysicsService() override = default;

        std::vector<Dependency> getDependencies() override {
            return {
                {"WorldRepository", &worldRepository},
                {"EngineRepository", &engineRepository}
            };
        }

        void onInitialize() override;

        void dispose() override;

        std::string getSyncThreadId() const override { return "physics_transform"; }

        void onAsyncSync() override;

    private:
        void startSimulation();

        void stopSimulation();

        void createBodies();

        void updateTransforms();
    };
}

#endif //METAL_ENGINE_PHYSICSSERVICE_H
