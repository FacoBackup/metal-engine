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
    }

    void AsyncSyncService::runSyncLoop(const std::string& threadId) {
        auto it = syncGroups.find(threadId);
        if (it == syncGroups.end()) return;

        const auto& services = it->second;

        while (running) {
            for (auto* service : services) {
                service->onAsyncSync();
            }
            // Small sleep to prevent 100% CPU usage if no work is needed
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}
