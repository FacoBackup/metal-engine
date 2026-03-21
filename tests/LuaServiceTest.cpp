#include <gtest/gtest.h>
#include "../src/ApplicationContext.h"
#include "../src/engine/service/LuaService.h"

using namespace Metal;

class LuaServiceTest : public ::testing::Test {
protected:
    std::unique_ptr<ApplicationContext> context;
    LuaService *luaService;

    void SetUp() override {
        context = std::make_unique<ApplicationContext>(true);
        auto service = std::make_shared<LuaService>();
        context->registerSingleton<LuaService>(service);
        service->onInitialize();
        luaService = service.get();
    }

    void TearDown() override {
        luaService->dispose();
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
