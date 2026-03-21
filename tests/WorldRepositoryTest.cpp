#include <gtest/gtest.h>
#include <entt/entt.hpp>
#include "../src/ApplicationContext.h"
#include "../src/engine/repository/WorldRepository.h"
#include "../src/engine/dto/MetadataComponent.h"
#include "../src/engine/service/RayTracingService.h"
#include "../src/editor/service/HistoryService.h"
#include "../src/core/DirectoryService.h"
#include "../src/engine/EngineContext.h"
#include "../src/ApplicationEventContext.h"
#include <iostream>

using namespace Metal;

class WorldRepositoryTest : public ::testing::Test, public IEventMember {
protected:
    std::unique_ptr<ApplicationContext> context;
    WorldRepository *repo;
    std::vector<std::string> eventLog;

    void SetUp() override {
        eventLog.clear();
        context = std::make_unique<ApplicationContext>(true);

        auto worldRepo = std::make_shared<WorldRepository>();
        auto rayTracingService = std::make_shared<RayTracingService>();
        auto directoryService = std::make_shared<DirectoryService>();
        auto engineContext = std::make_shared<EngineContext>();
        auto historyService = std::make_shared<HistoryService>();

        context->registerSingleton<WorldRepository>(worldRepo);
        context->registerSingleton<RayTracingService>(rayTracingService);
        context->registerSingleton<DirectoryService>(directoryService);
        context->registerSingleton<EngineContext>(engineContext);
        context->registerSingleton<HistoryService>(historyService);

        context->injectDependencies(worldRepo.get());

        repo = worldRepo.get();

        eventListener([this](const Event &e) {
            eventLog.push_back(e.key);
        }, "MetadataComponent");
        eventListener([this](const Event &e) {
            eventLog.push_back(e.key);
        }, "TransformComponent");
        eventListener([this](const Event &e) {
            eventLog.push_back(e.key);
        }, "PrimitiveComponent");
    }
};

TEST_F(WorldRepositoryTest, CreateEntityEvents) {
    repo->createEntity();
    // createEntity calls createComponent(METADATA)
    ASSERT_EQ(eventLog.size(), 1);
    EXPECT_EQ(eventLog[0], "MetadataComponent");
}

TEST_F(WorldRepositoryTest, CreateComponentEvents) {
    entt::entity entity = repo->createEntity();
    eventLog.clear();

    repo->createComponent(entity, TRANSFORM);
    ASSERT_EQ(eventLog.size(), 1);
    EXPECT_EQ(eventLog[0], "TransformComponent");
}

TEST_F(WorldRepositoryTest, UndoRedoEvents) {
    HistoryService &history = context->getSingleton<HistoryService>();

    entt::entity entity = repo->createEntity();
    repo->createComponent(entity, TRANSFORM);
    ASSERT_EQ(eventLog.size(), 2);
    EXPECT_EQ(eventLog[0], "MetadataComponent");
    EXPECT_EQ(eventLog[1], "TransformComponent");

    // Undo creation (Delete entity) - no component creation events expected here
    history.undo();
    eventLog.clear();

    // Redo creation (Restore entity) - calls deserializeEntityComplete
    history.redo();

    // Should have Metadata and Transformation events
    ASSERT_EQ(eventLog.size(), 2);
    EXPECT_EQ(eventLog[0], "TransformComponent");
    EXPECT_EQ(eventLog[1], "MetadataComponent");
}

TEST_F(WorldRepositoryTest, CreateEntity) {
    entt::entity entity = repo->createEntity();
    EXPECT_NE(entity, static_cast<entt::entity>(entt::null));
    EXPECT_TRUE(repo->registry.valid(entity));
}

TEST_F(WorldRepositoryTest, GetEntityMetadata) {
    entt::entity entity = repo->createEntity();
    MetadataComponent *metadata = repo->getEntity(entity);
    EXPECT_NE(metadata, nullptr);
}

TEST_F(WorldRepositoryTest, DeleteEntity) {
    entt::entity entity = repo->createEntity();
    repo->deleteEntities({entity});
    EXPECT_FALSE(repo->registry.valid(entity));
}

TEST_F(WorldRepositoryTest, ChangeVisibility) {
    entt::entity entity = repo->createEntity();

    repo->changeVisibility(entity, false);
    EXPECT_EQ(repo->hiddenEntities.count(entity), 1);
    EXPECT_TRUE(repo->hiddenEntities.at(entity));

    repo->changeVisibility(entity, true);
    EXPECT_EQ(repo->hiddenEntities.count(entity), 0);
}

TEST_F(WorldRepositoryTest, CreateEntityUndoRedo) {
    HistoryService &history = context->getSingleton<HistoryService>();

    entt::entity entity = repo->createEntity();
    EXPECT_TRUE(repo->registry.valid(entity));

    history.undo();
    EXPECT_FALSE(repo->registry.valid(entity));

    history.redo();
    EXPECT_TRUE(repo->registry.valid(entity));
}

TEST_F(WorldRepositoryTest, DeleteEntityUndoRedo) {
    HistoryService &history = context->getSingleton<HistoryService>();

    entt::entity entity = repo->createEntity();
    std::string name = "TestEntity";
    repo->getEntity(entity)->name = name;

    repo->deleteEntities({entity});
    EXPECT_FALSE(repo->registry.valid(entity));

    history.undo();
    EXPECT_TRUE(repo->registry.valid(entity));
    EXPECT_EQ(repo->getEntity(entity)->name, name);

    history.redo();
    EXPECT_FALSE(repo->registry.valid(entity));
}

TEST_F(WorldRepositoryTest, ChangeVisibilityUndoRedo) {
    HistoryService &history = context->getSingleton<HistoryService>();

    entt::entity entity = repo->createEntity();

    repo->changeVisibility(entity, false);
    EXPECT_TRUE(repo->hiddenEntities.contains(entity));

    history.undo();
    EXPECT_FALSE(repo->hiddenEntities.contains(entity));

    history.redo();
    EXPECT_TRUE(repo->hiddenEntities.contains(entity));
}
