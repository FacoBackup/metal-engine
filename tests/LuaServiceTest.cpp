#include <gtest/gtest.h>
#include <entt/entt.hpp>
#include <iostream>

#include <fstream>
#include <filesystem>
#include "../src/ApplicationContext.h"
#include "../src/engine/EngineContext.h"
#include "../src/engine/service/LuaService.h"
#include "../src/engine/repository/WorldRepository.h"
#include "../src/engine/dto/ScriptComponent.h"
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

        auto engineContext = std::make_shared<EngineContext>();
        engineContext->deltaTime = 0.016f;
        context->registerSingleton<EngineContext>(engineContext);

        auto service = std::make_shared<LuaService>();
        context->registerSingleton<LuaService>(service);
        
        context->injectDependencies(service.get());
        service->onInitialize();
        luaService = service.get();
    }

    void TearDown() override {
        if (fs::exists("test_script.lua")) fs::remove("test_script.lua");
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

TEST_F(LuaServiceTest, ScriptLifecycle) {
    createLuaFile("test_script.lua", 
        "local script = {}\n"
        "function script:onCreate() self.created = true end\n"
        "function script:onUpdate() self.updates = (self.updates or 0) + 1 end\n"
        "function script:onDestroy() self.destroyed = true end\n"
        "return script"
    );

    auto entity = worldRepository->registry.create();
    auto& comp = worldRepository->registry.emplace<ScriptComponent>(entity);
    comp.scriptPath = "test_script.lua";

    // Play event
    ApplicationEventContext::dispatch("Play");
    
    ASSERT_TRUE(comp.instance.valid());
    bool created = comp.instance["created"];
    EXPECT_TRUE(created);

    // Sync (Update)
    luaService->onSync();
    int updates = comp.instance["updates"];
    EXPECT_EQ(updates, 1);

    luaService->onSync();
    updates = comp.instance["updates"];
    EXPECT_EQ(updates, 2);

    // Stop event
    std::cout << "Dispatching Stop" << std::endl;
    ApplicationEventContext::dispatch("Stop");
    std::cout << "Checking destroyed" << std::endl;
    EXPECT_FALSE(comp.instance.valid());
}

TEST_F(LuaServiceTest, EntityIdAndWorldBindings) {
    auto entity = worldRepository->createEntity();
    auto& comp = worldRepository->registry.emplace<ScriptComponent>(entity);
    createLuaFile("test_script.lua", 
        "local script = {}\n"
        "function script:onUpdate()\n"
        "  self.captured_entity = self.this_entity\n"
        "  self.new_ent = world:createEntity()\n"
        "  self.has_metadata = world:hasComponent(self.new_ent, ComponentType.METADATA)\n"
        "end\n"
        "return script"
    );

    comp.scriptPath = "test_script.lua";

    ApplicationEventContext::dispatch("Play");
    luaService->onSync();

    ASSERT_TRUE(comp.instance.valid());
    entt::entity captured = comp.instance["captured_entity"];
    EXPECT_EQ(captured, entity);

    entt::entity newEnt = comp.instance["new_ent"];
    bool hasMetadata = comp.instance["has_metadata"];
    EXPECT_TRUE(hasMetadata);

    float dt = luaService->getState()["deltaTime"];
    EXPECT_GT(dt, -1.0f); // Just check it's bound
}

TEST_F(LuaServiceTest, ScriptIsolation) {
    createLuaFile("test_iso.lua", 
        "local script = {}\n"
        "function script:onCreate() my_var = (my_var or 0) + 1; self.val = my_var end\n"
        "return script"
    );

    auto ent1 = worldRepository->createEntity();
    auto& comp1 = worldRepository->registry.emplace<ScriptComponent>(ent1);
    comp1.scriptPath = "test_iso.lua";

    auto ent2 = worldRepository->createEntity();
    auto& comp2 = worldRepository->registry.emplace<ScriptComponent>(ent2);
    comp2.scriptPath = "test_iso.lua";

    ApplicationEventContext::dispatch("Play");

    int val1 = comp1.instance["val"];
    int val2 = comp2.instance["val"];

    // If they share global state 'my_var', val2 would be 2.
    // With isolation, both should be 1 as they have separate environments.
    EXPECT_EQ(val1, 1);
    EXPECT_EQ(val2, 1);

    if (fs::exists("test_iso.lua")) fs::remove("test_iso.lua");
    if (fs::exists("test_error.lua")) fs::remove("test_error.lua");
    if (fs::exists("test_async.lua")) fs::remove("test_async.lua");
}

TEST_F(LuaServiceTest, ScriptErrorHandling) {
    createLuaFile("test_error.lua", 
        "local script = {}\n"
        "function script:onUpdate() error('intentional error') end\n"
        "return script"
    );

    auto entity = worldRepository->createEntity();
    auto& comp = worldRepository->registry.emplace<ScriptComponent>(entity);
    comp.scriptPath = "test_error.lua";

    ApplicationEventContext::dispatch("Play");
    
    // First update should trigger error and disable script
    luaService->onSync();
    EXPECT_TRUE(comp.isErrored);

    // Further updates should not crash and should respect isErrored
    luaService->onSync();
    EXPECT_TRUE(comp.isErrored);
}

TEST_F(LuaServiceTest, ScriptAsyncExecution) {
    createLuaFile("test_async.lua", 
        "local script = {}\n"
        "function script:onUpdate() self.async_done = true end\n"
        "return script"
    );

    auto entity = worldRepository->createEntity();
    auto& comp = worldRepository->registry.emplace<ScriptComponent>(entity);
    comp.scriptPath = "test_async.lua";
    comp.runAsync = true;

    ApplicationEventContext::dispatch("Play");
    
    // onSync should NOT update this script
    luaService->onSync();
    bool done = comp.instance["async_done"].get_or(false);
    EXPECT_FALSE(done);

    // onAsyncSync SHOULD update this script
    luaService->onAsyncSync();
    done = comp.instance["async_done"];
    EXPECT_TRUE(done);
}

TEST_F(LuaServiceTest, ScriptReloadOnUpdate) {
    createLuaFile("test_v1.lua", "return { v = 1, onUpdate = function(self) end }");
    createLuaFile("test_v2.lua", "return { v = 2, onUpdate = function(self) end }");

    auto entity = worldRepository->createEntity();
    auto& comp = worldRepository->registry.emplace<ScriptComponent>(entity);
    comp.scriptPath = "test_v1.lua";

    ApplicationEventContext::dispatch("Play");
    int v = comp.instance["v"];
    EXPECT_EQ(v, 1);

    // Update path
    comp.scriptPath = "test_v2.lua";
    worldRepository->registry.patch<ScriptComponent>(entity);

    v = comp.instance["v"];
    EXPECT_EQ(v, 2);
}
