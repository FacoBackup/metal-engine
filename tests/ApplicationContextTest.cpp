#include <gtest/gtest.h>
#include <entt/entt.hpp>
#include "../src/ApplicationContext.h"
#include "../src/common/IContextMember.h"
#include <iostream>

using namespace Metal;

class MockContextMember : public IContextMember {
public:
    int value = 0;
};

TEST(ApplicationContextTest, RegisterAndGetSingleton) {
    ApplicationContext ctx(true);
    auto member = std::make_shared<MockContextMember>();
    member->value = 42;
    
    ctx.registerSingleton<MockContextMember>(member);
    
    MockContextMember& retrieved = ctx.getSingleton<MockContextMember>();
    EXPECT_EQ(retrieved.value, 42);
}

TEST(ApplicationContextTest, GetSingletonByName) {
    ApplicationContext ctx(true);
    auto member = std::make_shared<MockContextMember>();
    
    ctx.registerSingleton<MockContextMember>(member);
    
    void* ptr = ctx.getSingletonByName("MockContextMember");
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(static_cast<MockContextMember*>(ptr), member.get());
}

TEST(ApplicationContextTest, ThrowsWhenSingletonNotFound) {
    ApplicationContext ctx(true);
    EXPECT_THROW(ctx.getSingleton<MockContextMember>(), std::runtime_error);
}
