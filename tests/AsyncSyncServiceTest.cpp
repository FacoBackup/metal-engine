#include <gtest/gtest.h>
#include <ApplicationContext.h>
#include <core/service/ThreadManager.h>
#include <engine/service/AsyncSyncService.h>
#include <common/IAsyncSync.h>
#include <common/IService.h>
#include <atomic>
#include <chrono>
#include <thread>

using namespace Metal;

class MockAsyncService1 final : public IService, public IAsyncSync {
public:
    std::atomic<int> syncCount{0};
    std::string getSyncThreadId() const override { return "thread_1"; }
    float getSyncInterval() const override { return 1.0f; }
    void onAsyncSync() override { syncCount++; }
};

class MockAsyncService2 final : public IService, public IAsyncSync {
public:
    std::atomic<int> syncCount{0};
    std::string getSyncThreadId() const override { return "thread_2"; }
    float getSyncInterval() const override { return 1.0f; }
    void onAsyncSync() override { syncCount++; }
};

TEST(AsyncSyncServiceTest, OrchestratesAsyncSync) {
    auto context = std::make_shared<ApplicationContext>(true);

    auto threadManager = std::make_shared<ThreadManager>();
    auto asyncSyncService = std::make_shared<AsyncSyncService>();
    auto mockService1 = std::make_shared<MockAsyncService1>();
    auto mockService2 = std::make_shared<MockAsyncService2>();

    context->registerSingleton<ApplicationContext>(context);
    context->registerSingleton<ThreadManager>(threadManager);
    context->registerSingleton<AsyncSyncService>(asyncSyncService);
    context->registerSingleton<MockAsyncService1>(mockService1);
    context->registerSingleton<MockAsyncService2>(mockService2);

    context->onInitialize();

    // Give it some time to run
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_GT(mockService1->syncCount, 0);
    EXPECT_GT(mockService2->syncCount, 0);

    context->dispose();
}
