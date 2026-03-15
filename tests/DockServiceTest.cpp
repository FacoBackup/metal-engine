#include <gtest/gtest.h>
#include <memory>
#include "../src/editor/service/DockService.h"
#include "../src/editor/dto/DockDTO.h"
#include "../src/editor/dto/DockSpace.h"

namespace Metal {

class DockServiceTest : public ::testing::Test {
protected:
    std::unique_ptr<DockService> dockService;

    void SetUp() override {
        dockService = std::make_unique<DockService>();
    }
};

TEST_F(DockServiceTest, InitializationPopulatesVectors) {
    dockService->onInitialize();

    EXPECT_NE(dockService->getCenter(), nullptr);
    EXPECT_TRUE(dockService->getCenter()->isCenter);
    
    EXPECT_EQ(dockService->getLeft().size(), 2);
    EXPECT_EQ(dockService->getRight().size(), 2);
    EXPECT_EQ(dockService->getBottom().size(), 1);
}

TEST_F(DockServiceTest, RemoveDockRemovesFromLeft) {
    dockService->onInitialize();
    dockService->setIsInitialized(true);
    
    auto dockToRemove = dockService->getLeft()[0];
    dockService->removeDock(dockToRemove);
    
    EXPECT_EQ(dockService->getLeft().size(), 1);
    EXPECT_FALSE(dockService->getIsInitialized());
}

TEST_F(DockServiceTest, RemoveDockRemovesFromRight) {
    dockService->onInitialize();
    dockService->setIsInitialized(true);
    
    auto dockToRemove = dockService->getRight()[0];
    dockService->removeDock(dockToRemove);
    
    EXPECT_EQ(dockService->getRight().size(), 1);
    EXPECT_FALSE(dockService->getIsInitialized());
}

TEST_F(DockServiceTest, RemoveDockRemovesFromBottom) {
    dockService->onInitialize();
    dockService->setIsInitialized(true);
    
    auto dockToRemove = dockService->getBottom()[0];
    dockService->removeDock(dockToRemove);
    
    EXPECT_TRUE(dockService->getBottom().empty());
    EXPECT_FALSE(dockService->getIsInitialized());
}

TEST_F(DockServiceTest, RemoveCenterDockDoesNothing) {
    dockService->onInitialize();
    dockService->setIsInitialized(true);
    
    auto center = dockService->getCenter();
    dockService->removeDock(center);
    
    EXPECT_NE(dockService->getCenter(), nullptr);
    EXPECT_TRUE(dockService->getIsInitialized());
}

TEST_F(DockServiceTest, RemoveNullDockDoesNothing) {
    dockService->onInitialize();
    dockService->setIsInitialized(true);
    
    dockService->removeDock(nullptr);
    
    EXPECT_TRUE(dockService->getIsInitialized());
}

TEST_F(DockServiceTest, AddDocksIncrementsSizesAndResetsInitialization) {
    dockService->onInitialize();
    dockService->setIsInitialized(true);
    
    dockService->addLeftDock();
    EXPECT_EQ(dockService->getLeft().size(), 3);
    EXPECT_FALSE(dockService->getIsInitialized());
    
    dockService->setIsInitialized(true);
    dockService->addRightDock();
    EXPECT_EQ(dockService->getRight().size(), 3);
    EXPECT_FALSE(dockService->getIsInitialized());
    
    dockService->setIsInitialized(true);
    dockService->addBottomDock();
    EXPECT_EQ(dockService->getBottom().size(), 2);
    EXPECT_FALSE(dockService->getIsInitialized());
}

} // namespace Metal
