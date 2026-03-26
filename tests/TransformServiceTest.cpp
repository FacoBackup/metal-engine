#include <gtest/gtest.h>
#include <entt/entt.hpp>
#include "../src/ApplicationContext.h"
#include "../src/engine/repository/WorldRepository.h"
#include "../src/engine/service/DirtyStateService.h"
#include "../src/engine/service/TransformService.h"
#include "../src/engine/dto/TransformComponent.h"
#include "../src/engine/dto/PrimitiveComponent.h"
#include "../src/editor/dto/InspectableEventPayload.h"
#include "../src/ApplicationEventContext.h"
#include "../src/core/DirectoryService.h"
#include "../src/editor/service/HistoryService.h"

using namespace Metal;

class TransformServiceTest : public ::testing::Test {
protected:
    std::unique_ptr<ApplicationContext> context;
    WorldRepository *worldRepo;
    DirtyStateService *dirtyStateService;
    TransformService *transformService;

    void SetUp() override {
        context = std::make_unique<ApplicationContext>(true);

        auto worldRepoPtr = std::make_shared<WorldRepository>();
        auto dirtyStateServicePtr = std::make_shared<DirtyStateService>();
        auto transformServicePtr = std::make_shared<TransformService>();
        
        // Mock dependencies for WorldRepository
        auto directoryService = std::make_shared<DirectoryService>();
        auto historyService = std::make_shared<HistoryService>();

        context->registerSingleton<WorldRepository>(worldRepoPtr);
        context->registerSingleton<DirtyStateService>(dirtyStateServicePtr);
        context->registerSingleton<TransformService>(transformServicePtr);
        context->registerSingleton<DirectoryService>(directoryService);
        context->registerSingleton<HistoryService>(historyService);

        context->injectDependencies(worldRepoPtr.get());
        context->injectDependencies(transformServicePtr.get());

        worldRepo = worldRepoPtr.get();
        dirtyStateService = dirtyStateServicePtr.get();
        transformService = transformServicePtr.get();

        transformService->onInitialize();
    }
};

TEST_F(TransformServiceTest, EventDispatchingMarksDirty) {
    entt::entity entity = worldRepo->createEntity();
    worldRepo->createComponent(entity, TRANSFORM);
    auto& transform = worldRepo->registry.get<TransformComponent>(entity);

    // Initial state: transformService->onInitialize() marks all existing entities with TransformComponent as dirty.
    // Let's clear them first to test the event.
    dirtyStateService->getDirtyEntities(DirtyType::Transform, true);
    ASSERT_FALSE(dirtyStateService->isEntityDirty(entity, DirtyType::Transform));

    // Dispatch event
    auto payload = std::make_shared<InspectableEventPayload>(&transform);
    ApplicationEventContext::dispatch("TransformComponent", payload);

    // Check if marked dirty
    EXPECT_TRUE(dirtyStateService->isEntityDirty(entity, DirtyType::Transform));
}

TEST_F(TransformServiceTest, ConsumptionOfDirtyFlagsAndTransformation) {
    entt::entity entity = worldRepo->createEntity();
    worldRepo->createComponent(entity, TRANSFORM);
    auto& transform = worldRepo->registry.get<TransformComponent>(entity);
    
    // Set some values
    transform.translation = glm::vec3(1.0f, 2.0f, 3.0f);
    transform.isStatic = false; // Allow transformation
    
    // Mark dirty (simulating event or initial state)
    dirtyStateService->markEntityDirty(entity, DirtyType::Transform);
    
    // Initial model should be identity (or not yet updated)
    EXPECT_EQ(transform.model, glm::mat4(1.0f));

    // Run service
    transformService->onAsyncSync();

    // Check if dirty flag was consumed
    EXPECT_FALSE(dirtyStateService->isEntityDirty(entity, DirtyType::Transform));

    // Check if transformation occurred
    EXPECT_NE(transform.model, glm::mat4(1.0f));
    EXPECT_EQ(transform.model[3], glm::vec4(1.0f, 2.0f, 3.0f, 1.0f));
}

TEST_F(TransformServiceTest, BVHFlagBehavior) {
    entt::entity entity = worldRepo->createEntity();
    worldRepo->createComponent(entity, TRANSFORM);
    worldRepo->createComponent(entity, PRIMITIVE);
    
    auto& transform = worldRepo->registry.get<TransformComponent>(entity);
    transform.isStatic = false;

    // Ensure BVH is not dirty initially
    dirtyStateService->consumeDirtyFlags(DirtyType::BVH);
    ASSERT_FALSE(dirtyStateService->isDirty(DirtyType::BVH));

    // Mark for transformation
    dirtyStateService->markEntityDirty(entity, DirtyType::Transform);

    // Run service
    transformService->onAsyncSync();

    // BVH should now be dirty because entity has PRIMITIVE
    EXPECT_TRUE(dirtyStateService->isDirty(DirtyType::BVH));

    // Consume and check again
    dirtyStateService->consumeDirtyFlags(DirtyType::BVH);
    
    // Transform again
    dirtyStateService->markEntityDirty(entity, DirtyType::Transform);
    transformService->onAsyncSync();
    
    // Should be dirty again
    EXPECT_TRUE(dirtyStateService->isDirty(DirtyType::BVH));
}

TEST_F(TransformServiceTest, StaticEntityNoTransform) {
    entt::entity entity = worldRepo->createEntity();
    worldRepo->createComponent(entity, TRANSFORM);
    auto& transform = worldRepo->registry.get<TransformComponent>(entity);
    
    transform.translation = glm::vec3(5.0f);
    transform.isStatic = true; // Should prevent transform
    transform.forceTransform = false;

    dirtyStateService->markEntityDirty(entity, DirtyType::Transform);
    
    transformService->onAsyncSync();

    // Transformation should NOT have occurred (model stays identity)
    EXPECT_EQ(transform.model, glm::mat4(1.0f));
}
