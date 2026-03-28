#include "ThreadManager.h"

#include <ranges>

namespace Metal {
    std::thread &ThreadManager::getThread(const std::string &id, const std::function<void()> &task) {
        std::lock_guard lock(threadsMutex);

        if (threads.contains(id)) {
            return threads.at(id);
        }

        threads.emplace(id, task);
        return threads.at(id);
    }

    void ThreadManager::dispose() {
        std::lock_guard lock(threadsMutex);

        for (auto &thread: threads | std::views::values) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        threads.clear();
    }
}
