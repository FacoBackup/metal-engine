#include <gtest/gtest.h>
#include <engine/service/DirtyStateService.h>
#include <engine/enum/DirtyType.h>
#include <entt/entt.hpp>

using namespace Metal;

class DirtyStateServiceTest : public ::testing::Test {
protected:
    DirtyStateService service;
};

TEST_F(DirtyStateServiceTest, GlobalFlagsBasic) {
    // Initially, the Camera flag should not be dirty
    EXPECT_FALSE(service.isDirty(DirtyType::Camera));
    
    // Marking the Camera as dirty should set the flag
    service.markDirty(DirtyType::Camera);
    EXPECT_TRUE(service.isDirty(DirtyType::Camera));
    
    // Marking one flag should not affect other unrelated flags
    EXPECT_FALSE(service.isDirty(DirtyType::Environment));
    
    // Marking a second flag should work independently
    service.markDirty(DirtyType::Environment);
    EXPECT_TRUE(service.isDirty(DirtyType::Camera));
    EXPECT_TRUE(service.isDirty(DirtyType::Environment));
}

TEST_F(DirtyStateServiceTest, ConsumeGlobalFlags) {
    // Set multiple dirty flags
    service.markDirty(DirtyType::Camera);
    service.markDirty(DirtyType::Environment);
    
    // Consuming only the Camera flag should return it and clear it
    DirtyType consumed = service.consumeDirtyFlags(DirtyType::Camera);
    EXPECT_EQ(consumed, DirtyType::Camera);
    EXPECT_FALSE(service.isDirty(DirtyType::Camera));
    
    // Other flags (Environment) should remain dirty
    EXPECT_TRUE(service.isDirty(DirtyType::Environment));
    
    // Consuming remaining flags using the 'All' mask
    consumed = service.consumeDirtyFlags(DirtyType::All);
    EXPECT_EQ(consumed, DirtyType::Environment);
    EXPECT_FALSE(service.isDirty(DirtyType::Environment));
}

TEST_F(DirtyStateServiceTest, EntityFlagsBasic) {
    entt::registry registry;
    auto e1 = registry.create();
    auto e2 = registry.create();
    
    // Initially, entities should not have any dirty flags
    EXPECT_FALSE(service.isEntityDirty(e1, DirtyType::Transform));
    
    // Marking e1's Transform as dirty should not affect its Material or other entities
    service.markEntityDirty(e1, DirtyType::Transform);
    EXPECT_TRUE(service.isEntityDirty(e1, DirtyType::Transform));
    EXPECT_FALSE(service.isEntityDirty(e1, DirtyType::Material));
    EXPECT_FALSE(service.isEntityDirty(e2, DirtyType::Transform));
    
    // Marking e2 as dirty should be tracked independently
    service.markEntityDirty(e2, DirtyType::Material);
    EXPECT_TRUE(service.isEntityDirty(e2, DirtyType::Material));
}

TEST_F(DirtyStateServiceTest, ConsumeEntityFlags) {
    entt::registry registry;
    auto e1 = registry.create();
    
    // Set multiple dirty flags for the same entity
    service.markEntityDirty(e1, DirtyType::Transform | DirtyType::Material);
    
    // Consuming only the Transform flag for e1
    DirtyType consumed = service.consumeEntityDirtyFlags(e1, DirtyType::Transform);
    EXPECT_EQ(consumed, DirtyType::Transform);
    EXPECT_FALSE(service.isEntityDirty(e1, DirtyType::Transform));
    
    // The Material flag should still be dirty for e1
    EXPECT_TRUE(service.isEntityDirty(e1, DirtyType::Material));
    
    // Consuming the remaining Material flag
    consumed = service.consumeEntityDirtyFlags(e1, DirtyType::Material);
    EXPECT_EQ(consumed, DirtyType::Material);
    EXPECT_FALSE(service.isEntityDirty(e1, DirtyType::Material));
}

TEST_F(DirtyStateServiceTest, HasAnyEntityDirty) {
    entt::registry registry;
    auto e1 = registry.create();
    
    // Check global entity dirty status for a specific type
    EXPECT_FALSE(service.hasAnyEntityDirty(DirtyType::Transform));
    
    // Marking any entity as dirty should trigger the global check for that type
    service.markEntityDirty(e1, DirtyType::Transform);
    EXPECT_TRUE(service.hasAnyEntityDirty(DirtyType::Transform));
    EXPECT_FALSE(service.hasAnyEntityDirty(DirtyType::Material));
    
    // After consuming the flags for that entity, the global check should return false again
    service.consumeEntityDirtyFlags(e1, DirtyType::Transform);
    EXPECT_FALSE(service.hasAnyEntityDirty(DirtyType::Transform));
}

TEST_F(DirtyStateServiceTest, GetDirtyEntities) {
    entt::registry registry;
    auto e1 = registry.create();
    auto e2 = registry.create();
    auto e3 = registry.create();
    
    // Setup entities with different dirty types
    service.markEntityDirty(e1, DirtyType::Transform);
    service.markEntityDirty(e2, DirtyType::Material);
    service.markEntityDirty(e3, DirtyType::Transform | DirtyType::Material);
    
    // Retrieve all entities that have the Transform flag dirty without consuming them
    auto transformDirty = service.getDirtyEntities(DirtyType::Transform, false);
    EXPECT_EQ(transformDirty.size(), 2);
    EXPECT_TRUE(std::find(transformDirty.begin(), transformDirty.end(), e1) != transformDirty.end());
    EXPECT_TRUE(std::find(transformDirty.begin(), transformDirty.end(), e3) != transformDirty.end());
    
    // Retrieve and consume entities that have the Material flag dirty
    auto materialDirty = service.getDirtyEntities(DirtyType::Material, true);
    EXPECT_EQ(materialDirty.size(), 2);
    EXPECT_TRUE(std::find(materialDirty.begin(), materialDirty.end(), e2) != materialDirty.end());
    EXPECT_TRUE(std::find(materialDirty.begin(), materialDirty.end(), e3) != materialDirty.end());
    
    // After consuming Material flags, e2 should be clean of Material, and e3 should only have Transform
    EXPECT_FALSE(service.isEntityDirty(e2, DirtyType::Material));
    EXPECT_FALSE(service.isEntityDirty(e3, DirtyType::Material));
    EXPECT_TRUE(service.isEntityDirty(e3, DirtyType::Transform)); // Transform flag was not consumed
}

TEST_F(DirtyStateServiceTest, ClearEntityFlags) {
    entt::registry registry;
    auto e1 = registry.create();
    service.markEntityDirty(e1, DirtyType::Transform);
    
    // clearEntityFlags should remove all entries for all entities
    service.clearEntityFlags();
    EXPECT_FALSE(service.isEntityDirty(e1, DirtyType::Transform));
    EXPECT_FALSE(service.hasAnyEntityDirty(DirtyType::All));
}
