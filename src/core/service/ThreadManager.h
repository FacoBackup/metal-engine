#ifndef METAL_ENGINE_THREADMANAGER_H
#define METAL_ENGINE_THREADMANAGER_H

#include <unordered_map>
#include <string>
#include <thread>
#include <mutex>
#include <functional>
#include <memory>
#include <vector>

#include <common/IService.h>
#include <common/IDisposable.h>

namespace Metal {
    class ThreadManager final : public IService, public IDisposable {
    public:
        ~ThreadManager() override = default;

        std::thread& getThread(const std::string& id, const std::function<void()>& task);

        void dispose() override;

    private:
        std::unordered_map<std::string, std::thread> threads;
        std::mutex threadsMutex;
    };
}

#endif //METAL_ENGINE_THREADMANAGER_H
