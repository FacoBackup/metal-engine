#include <engine/service/AsyncSyncService.h>
#include <core/service/ThreadManager.h>
#include <common/IAsyncSync.h>
#include <ApplicationContext.h>
#include <set>
#include <thread>
#include <chrono>

namespace Metal {
    void AsyncSyncService::onInitialize() {
        if (!ctx) return;

        auto asyncSyncServices = ctx->getSingletons<IAsyncSync>();
        syncGroups.clear();

        for (auto* service : asyncSyncServices) {
            std::string threadId = service->getSyncThreadId();
            syncGroups[threadId].push_back(service);
        }

        for (const auto& [threadId, services] : syncGroups) {
            threadManager->getThread(threadId, [this, threadId]() {
                this->runSyncLoop(threadId);
            });
        }
    }

    void AsyncSyncService::dispose() {
        running = false;
        shutdownCv.notify_all();
    }

    void AsyncSyncService::runSyncLoop(const std::string& threadId) {
        auto it = syncGroups.find(threadId);
        if (it == syncGroups.end()) return;

        const auto& services = it->second;

        while (running) {
            float totalInterval = 0.f;
            for (auto* service : services) {
                service->onAsyncSync();
                totalInterval += service->getSyncInterval();
            }
            
            float averageInterval = totalInterval / static_cast<float>(services.size());
            if (averageInterval > 0) {
                std::unique_lock lock(shutdownMutex);
                shutdownCv.wait_for(lock, std::chrono::milliseconds(static_cast<long long>(averageInterval)), [this] {
                    return !running.load();
                });
            } else {
                // Fallback to small sleep to prevent 100% CPU usage if no interval is provided
                std::unique_lock lock(shutdownMutex);
                shutdownCv.wait_for(lock, std::chrono::milliseconds(1), [this] {
                    return !running.load();
                });
            }
        }
    }
}
