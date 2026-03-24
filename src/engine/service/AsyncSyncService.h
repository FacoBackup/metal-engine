#ifndef METAL_ENGINE_ASYNCSYNCSERVICE_H
#define METAL_ENGINE_ASYNCSYNCSERVICE_H

#include <common/IService.h>
#include <common/IInit.h>
#include <common/IDisposable.h>
#include <vector>
#include <atomic>
#include <unordered_map>
#include <string>

namespace Metal {
    class ThreadManager;

    class IAsyncSync;

    class AsyncSyncService final : public IService, public IInit, public IDisposable {
    public:
        ~AsyncSyncService() override = default;

        std::vector<Dependency> getDependencies() override {
            return {
                {"ThreadManager", &threadManager}
            };
        }

        void onInitialize() override;

        void dispose() override;

    private:
        ThreadManager *threadManager = nullptr;
        std::atomic<bool> running{true};
        std::unordered_map<std::string, std::vector<IAsyncSync *> > syncGroups;

        void runSyncLoop(const std::string &threadId);
    };
}

#endif //METAL_ENGINE_ASYNCSYNCSERVICE_H
