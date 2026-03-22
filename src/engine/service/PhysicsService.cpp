#include "PhysicsService.h"
#include "../repository/WorldRepository.h"
#include "../repository/EngineRepository.h"
#include "../dto/RigidBodyComponent.h"
#include "../dto/BoxColliderComponent.h"
#include "../dto/SphereColliderComponent.h"
#include "../dto/CapsuleColliderComponent.h"
#include "../dto/MeshColliderComponent.h"
#include "../dto/TransformComponent.h"
#include "../../ApplicationEventContext.h"

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

namespace Metal {

    // Jolt implementation requirements
    class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
    public:
        BPLayerInterfaceImpl() {
            mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
            mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
        }

        uint32_t GetNumBroadPhaseLayers() const override { return BroadPhaseLayers::NUM_LAYERS; }

        JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override {
            return mObjectToBroadPhase[inLayer];
        }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
        const char *GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override {
            switch ((JPH::BroadPhaseLayer::Type)inLayer) {
                case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING: return "NON_MOVING";
                case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING: return "MOVING";
                default: return "INVALID";
            }
        }
#endif
    private:
        JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
    };

    class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter {
    public:
        bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override {
            switch (inLayer1) {
                case Layers::NON_MOVING: return inLayer2 == BroadPhaseLayers::MOVING;
                case Layers::MOVING: return true;
                default: return false;
            }
        }
    };

    class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter {
    public:
        bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override {
            switch (inObject1) {
                case Layers::NON_MOVING: return inObject2 == Layers::MOVING;
                case Layers::MOVING: return true;
                default: return false;
            }
        }
    };

    void PhysicsService::onInitialize() {
        JPH::RegisterDefaultAllocator();
        JPH::Factory::sInstance = new JPH::Factory();
        JPH::RegisterTypes();

        tempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);
        jobSystem = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

        physicsSystem = new JPH::PhysicsSystem();
        
        static BPLayerInterfaceImpl bp_layer_interface;
        static ObjectVsBroadPhaseLayerFilterImpl obj_vs_bp_layer_filter;
        static ObjectLayerPairFilterImpl obj_vs_obj_layer_filter;

        physicsSystem->Init(1024, 0, 1024, 1024, bp_layer_interface, obj_vs_bp_layer_filter, obj_vs_obj_layer_filter);

        eventListener([this](const Event &) {
            startSimulation();
        }, "Play");

        eventListener([this](const Event &) {
            stopSimulation();
        }, "Stop");
    }

    void PhysicsService::dispose() {
        stopSimulation();
        delete physicsSystem;
        delete jobSystem;
        delete tempAllocator;
        delete JPH::Factory::sInstance;
    }

    void PhysicsService::onSync() {
        if (!enabled) return;

        physicsSystem->Update(1.0f / 60.0f, 1, tempAllocator, jobSystem);
        updateTransforms();
    }

    void PhysicsService::startSimulation() {
        physicsSystem->SetGravity(JPH::Vec3(engineRepository->gravity.x, engineRepository->gravity.y, engineRepository->gravity.z));
        createBodies();
        enabled = true;
    }

    void PhysicsService::stopSimulation() {
        enabled = false;
        JPH::BodyInterface &bodyInterface = physicsSystem->GetBodyInterface();
        for (auto const& [entity, bodyID] : entityToBody) {
            bodyInterface.RemoveBody(bodyID);
            bodyInterface.DestroyBody(bodyID);
        }
        entityToBody.clear();
    }

    void PhysicsService::createBodies() {
        JPH::BodyInterface &bodyInterface = physicsSystem->GetBodyInterface();
        auto view = worldRepository->registry.view<RigidBodyComponent, TransformComponent>();

        for (auto entity : view) {
            auto &rb = view.get<RigidBodyComponent>(entity);
            auto &trans = view.get<TransformComponent>(entity);

            JPH::ShapeRefC shape;
            
            if (auto *box = worldRepository->registry.try_get<BoxColliderComponent>(entity)) {
                shape = new JPH::BoxShape(JPH::Vec3(box->halfExtents.x, box->halfExtents.y, box->halfExtents.z));
            } else if (auto *sphere = worldRepository->registry.try_get<SphereColliderComponent>(entity)) {
                shape = new JPH::SphereShape(sphere->radius);
            } else if (auto *capsule = worldRepository->registry.try_get<CapsuleColliderComponent>(entity)) {
                shape = new JPH::CapsuleShape(capsule->height * 0.5f, capsule->radius);
            } else {
                // Default to a small box if no collider is found
                shape = new JPH::BoxShape(JPH::Vec3(0.1f, 0.1f, 0.1f));
            }

            JPH::EMotionType motionType = rb.isKinematic ? JPH::EMotionType::Kinematic : JPH::EMotionType::Dynamic;
            JPH::ObjectLayer layer = rb.isKinematic ? Layers::NON_MOVING : Layers::MOVING;

            JPH::Quat joltRotation(trans.rotation.x, trans.rotation.y, trans.rotation.z, trans.rotation.w);
            JPH::BodyCreationSettings settings(shape, JPH::RVec3(trans.translation.x, trans.translation.y, trans.translation.z), joltRotation, motionType, layer);
            settings.mLinearDamping = rb.linearDamping;
            settings.mAngularDamping = rb.angularDamping;
            settings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
            settings.mMassPropertiesOverride.mMass = rb.mass;
            settings.mGravityFactor = rb.useGravity ? 1.0f : 0.0f;

            JPH::Body *body = bodyInterface.CreateBody(settings);
            bodyInterface.AddBody(body->GetID(), JPH::EActivation::Activate);
            entityToBody[entity] = body->GetID();
        }
    }

    void PhysicsService::updateTransforms() {
        JPH::BodyInterface &bodyInterface = physicsSystem->GetBodyInterface();
        for (auto& [entity, bodyID] : entityToBody) {
            if (bodyInterface.GetMotionType(bodyID) == JPH::EMotionType::Static) continue;

            JPH::RVec3 position;
            JPH::Quat rotation;
            bodyInterface.GetPositionAndRotation(bodyID, position, rotation);

            if (auto *trans = worldRepository->registry.try_get<TransformComponent>(entity)) {
                trans->translation = {position.GetX(), position.GetY(), position.GetZ()};
                trans->rotation = {rotation.GetX(), rotation.GetY(), rotation.GetZ(), rotation.GetW()};
                trans->forceTransform = true; // Signal that transform was updated
            }
        }
    }
}
