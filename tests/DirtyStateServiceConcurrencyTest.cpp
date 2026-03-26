#include <gtest/gtest.h>
#include "engine/service/DirtyStateService.h"
#include "engine/enum/DirtyType.h"
#include <thread>
#include <vector>
#include <atomic>
#include <random>

using namespace Metal;

/**
 * @test DirtyStateServiceConcurrencyTest - Stress tests the thread-safety of DirtyStateService.
 */
class DirtyStateServiceConcurrencyTest : public ::testing::Test {
protected:
    DirtyStateService dirtyStateService;
};

/**
 * @test Verifies that multiple threads can concurrently mark and consume global dirty flags.
 */
TEST_F(DirtyStateServiceConcurrencyTest, ConcurrentGlobalFlags) {
    const int threadCount = 8;
    const int iterations = 1000;
    std::vector<std::jthread> threads;

    std::atomic<bool> start{false};

    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&, i]() {
            while (!start) std::this_thread::yield();

            for (int j = 0; j < iterations; ++j) {
                // Alternately mark and consume different flags
                if (j % 2 == 0) {
                    dirtyStateService.markDirty(DirtyType::Camera);
                    dirtyStateService.markDirty(DirtyType::Environment);
                } else {
                    dirtyStateService.consumeDirtyFlags(DirtyType::Camera);
                    dirtyStateService.consumeDirtyFlags(DirtyType::Environment);
                }
            }
        });
    }

    start = true;
    threads.clear(); // Wait for all threads to finish

    // After many random operations, we just ensure no crashes or deadlocks occurred
    // and the service is still in a valid state.
    dirtyStateService.markDirty(DirtyType::GlobalSettings);
    EXPECT_TRUE(dirtyStateService.isDirty(DirtyType::GlobalSettings));
    EXPECT_NE(static_cast<uint32_t>(dirtyStateService.consumeDirtyFlags(DirtyType::GlobalSettings)), 0u);
    EXPECT_FALSE(dirtyStateService.isDirty(DirtyType::GlobalSettings));
}

/**
 * @test Verifies that multiple threads can concurrently mark and consume entity-specific dirty flags.
 */
TEST_F(DirtyStateServiceConcurrencyTest, ConcurrentEntityFlags) {
    const int threadCount = 8;
    const int iterations = 1000;
    const int entityRange = 100; // Stress many entities
    std::vector<std::jthread> threads;

    std::atomic<bool> start{false};

    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&, i]() {
            while (!start) std::this_thread::yield();

            std::mt19937 gen(i);
            std::uniform_int_distribution<> dis(1, entityRange);

            for (int j = 0; j < iterations; ++j) {
                entt::entity entity = static_cast<entt::entity>(dis(gen));
                
                // Randomly perform different operations on entity flags
                int op = j % 3;
                if (op == 0) {
                    dirtyStateService.markEntityDirty(entity, DirtyType::Transform);
                } else if (op == 1) {
                    dirtyStateService.isEntityDirty(entity, DirtyType::Transform);
                } else {
                    dirtyStateService.consumeEntityDirtyFlags(entity, DirtyType::Transform);
                }
            }
        });
    }

    start = true;
    threads.clear();

    // Final state check - service should be usable
    entt::entity testEntity = static_cast<entt::entity>(999);
    dirtyStateService.markEntityDirty(testEntity, DirtyType::Material);
    EXPECT_TRUE(dirtyStateService.isEntityDirty(testEntity, DirtyType::Material));
    EXPECT_NE(static_cast<uint32_t>(dirtyStateService.consumeEntityDirtyFlags(testEntity, DirtyType::Material)), 0u);
    EXPECT_FALSE(dirtyStateService.isEntityDirty(testEntity, DirtyType::Material));
}

/**
 * @test Verifies that getDirtyEntities can be called while other threads are marking entities.
 */
TEST_F(DirtyStateServiceConcurrencyTest, ConcurrentGetDirtyEntities) {
    const int markerThreads = 4;
    const int readerThreads = 2;
    const int iterations = 500;
    const int entityRange = 50;

    std::atomic<bool> start{false};

    // Use a separate vector for marker threads so we can wait for them specifically
    std::vector<std::jthread> markers;
    std::vector<std::jthread> readers;

    for (int i = 0; i < markerThreads; ++i) {
        markers.emplace_back([&, i]() {
            while (!start) std::this_thread::yield();
            std::mt19937 gen(i);
            std::uniform_int_distribution<> dis(1, entityRange);

            for (int j = 0; j < iterations; ++j) {
                entt::entity entity = static_cast<entt::entity>(dis(gen));
                dirtyStateService.markEntityDirty(entity, DirtyType::Transform);
                std::this_thread::yield();
            }
        });
    }

    for (int i = 0; i < readerThreads; ++i) {
        readers.emplace_back([&, i](std::stop_token stopToken) {
            while (!start) std::this_thread::yield();
            while (!stopToken.stop_requested()) {
                auto dirtyEntities = dirtyStateService.getDirtyEntities(DirtyType::Transform, true);
                for (auto entity : dirtyEntities) {
                    (void)entity;
                }
                std::this_thread::yield();
            }
        });
    }

    start = true;
    
    // Wait for all markers to finish their iterations
    markers.clear(); 
    
    // Now tell readers to stop
    readers.clear(); 

    // Final check
    dirtyStateService.markEntityDirty(static_cast<entt::entity>(123), DirtyType::Transform);
    EXPECT_TRUE(dirtyStateService.isEntityDirty(static_cast<entt::entity>(123), DirtyType::Transform));
}

/**
 * @test Verifies that clearing all entity flags is thread-safe.
 */
TEST_F(DirtyStateServiceConcurrencyTest, ConcurrentClearAllEntityFlags) {
    const int threadCount = 4;
    const int iterations = 100;
    std::vector<std::jthread> threads;

    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < iterations; ++j) {
                dirtyStateService.markEntityDirty(static_cast<entt::entity>(j), DirtyType::Mesh);
                if (j % 10 == 0) {
                    dirtyStateService.clearEntityFlags();
                }
            }
        });
    }
    
    threads.clear();
    
    // Clear once more after all threads have finished their last operations
    dirtyStateService.clearEntityFlags();
    EXPECT_FALSE(dirtyStateService.hasAnyEntityDirty(DirtyType::All));
}
