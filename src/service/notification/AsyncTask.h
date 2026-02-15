#ifndef METAL_ENGINE_ASYNCTASK_H
#define METAL_ENGINE_ASYNCTASK_H
#include <string>
#include <vector>
#include <mutex>
#include <atomic>
#include <functional>

namespace Metal {
    struct AsyncTask {
        std::string id;
        std::string name;
        std::atomic<float> progress{0.0f}; // 0.0 to 1.0, -1.0 for indeterminate
        std::atomic<bool> completed{false};
        std::function<void()> cancel;
    };
}
#endif //METAL_ENGINE_ASYNCTASK_H
