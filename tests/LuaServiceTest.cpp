#include <gtest/gtest.h>
#include <entt/entt.hpp>

#include <fstream>
#include <filesystem>
#include "../src/ApplicationContext.h"
#include "../src/engine/service/LuaService.h"
#include "../src/engine/repository/WorldRepository.h"
#include "../src/engine/dto/ScopedScriptComponent.h"
#include "../src/engine/dto/GlobalScriptComponent.h"
#include "../src/engine/dto/MetadataComponent.h"
#include "../src/ApplicationEventContext.h"

using namespace Metal;
namespace fs = std::filesystem;

class LuaServiceTest : public ::testing::Test {
protected:
    std::unique_ptr<ApplicationContext> context;
    LuaService *luaService;
    std::shared_ptr<WorldRepository> worldRepository;

    void SetUp() override {
        context = std::make_unique<ApplicationContext>(true);
        
        worldRepository = std::make_shared<WorldRepository>();
        context->registerSingleton<WorldRepository>(worldRepository);

        auto service = std::make_shared<LuaService>();
        context->registerSingleton<LuaService>(service);
        
        context->injectDependencies(service.get());
        service->onInitialize();
        luaService = service.get();
    }

    void TearDown() override {
        luaService->dispose();
        // Cleanup any created lua files
        if (fs::exists("test_create.lua")) fs::remove("test_create.lua");
        if (fs::exists("test_update.lua")) fs::remove("test_update.lua");
        if (fs::exists("test_destroy.lua")) fs::remove("test_destroy.lua");
        if (fs::exists("test_global.lua")) fs::remove("test_global.lua");
    }

    void createLuaFile(const std::string& path, const std::string& content) {
        std::ofstream ofs(path);
        ofs << content;
        ofs.close();
    }
};

TEST_F(LuaServiceTest, ExecuteSimpleString) {
    EXPECT_TRUE(luaService->executeString("a = 10 + 5"));
    int a = luaService->getState()["a"];
    EXPECT_EQ(a, 15);
}

TEST_F(LuaServiceTest, ExecuteInvalidString) {
    // This should return false and log an error (not crash)
    EXPECT_FALSE(luaService->executeString("this is not lua code"));
}

TEST_F(LuaServiceTest, CppBinding) {
    luaService->getState().set_function("cpp_add", [](int a, int b) {
        return a + b;
    });

    EXPECT_TRUE(luaService->executeString("res = cpp_add(20, 30)"));
    int res = luaService->getState()["res"];
    EXPECT_EQ(res, 50);
}

TEST_F(LuaServiceTest, PlayEventExecutesScripts) {
    createLuaFile("test_create.lua", "create_called = true");
    createLuaFile("test_global.lua", "global_called = true");

    auto entity = worldRepository->registry.create();
    auto& scoped = worldRepository->registry.emplace<ScopedScriptComponent>(entity);
    scoped.onCreatePath = "test_create.lua";

    auto globalEntity = worldRepository->registry.create();
    auto& global = worldRepository->registry.emplace<GlobalScriptComponent>(globalEntity);
    global.scriptPath = "test_global.lua";

    ApplicationEventContext::dispatch("Play");

    bool createCalled = luaService->getState()["create_called"];
    bool globalCalled = luaService->getState()["global_called"];

    EXPECT_TRUE(createCalled);
    EXPECT_TRUE(globalCalled);
}

TEST_F(LuaServiceTest, StopEventExecutesScripts) {
    createLuaFile("test_destroy.lua", "destroy_called = true");

    auto entity = worldRepository->registry.create();
    auto& scoped = worldRepository->registry.emplace<ScopedScriptComponent>(entity);
    scoped.onDestroyPath = "test_destroy.lua";

    ApplicationEventContext::dispatch("Stop");

    bool destroyCalled = luaService->getState()["destroy_called"];
    EXPECT_TRUE(destroyCalled);
}

TEST_F(LuaServiceTest, OnSyncExecutesScriptsWhenPlaying) {
    createLuaFile("test_update.lua", "update_count = (update_count or 0) + 1");

    auto entity = worldRepository->registry.create();
    auto& scoped = worldRepository->registry.emplace<ScopedScriptComponent>(entity);
    scoped.onUpdatePath = "test_update.lua";

    // Not playing yet
    luaService->onSync();
    sol::optional<int> countBefore = luaService->getState()["update_count"];
    EXPECT_FALSE(countBefore.has_value());

    ApplicationEventContext::dispatch("Play");
    
    luaService->onSync();
    int countAfter1 = luaService->getState()["update_count"];
    EXPECT_EQ(countAfter1, 1);

    luaService->onSync();
    int countAfter2 = luaService->getState()["update_count"];
    EXPECT_EQ(countAfter2, 2);

    ApplicationEventContext::dispatch("Stop");
    
    luaService->onSync();
    int countAfterStop = luaService->getState()["update_count"];
    EXPECT_EQ(countAfterStop, 2); // Should not have increased
}

TEST_F(LuaServiceTest, EntityIdAndWorldBindings) {
    auto entity = worldRepository->createEntity();
    auto& scoped = worldRepository->registry.emplace<ScopedScriptComponent>(entity);
    createLuaFile("test_entity_id.lua", "captured_entity = this_entity\n"
                                        "new_ent = world:createEntity()\n"
                                        "metadata = world:getEntity(new_ent)\n"
                                        "has_metadata = world:hasComponent(new_ent, 3)"); // 3 is METADATA

    scoped.onUpdatePath = "test_entity_id.lua";

    ApplicationEventContext::dispatch("Play");
    luaService->onSync();

    entt::entity captured = luaService->getState()["captured_entity"];
    EXPECT_EQ(captured, entity);

    entt::entity newEnt = luaService->getState()["new_ent"];
    // EXPECT_NE(newEnt, entt::null);
    bool hasMetadata = luaService->getState()["has_metadata"];
    EXPECT_TRUE(hasMetadata);

    if (fs::exists("test_entity_id.lua")) fs::remove("test_entity_id.lua");
}
