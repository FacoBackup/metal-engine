#ifndef METAL_ENGINE_ASYNCTASKSERVICE_H
#define METAL_ENGINE_ASYNCTASKSERVICE_H

#include <string>
#include <vector>
#include <mutex>
#include <functional>
#include "../dto/AsyncTask.h"

namespace Metal {
    class AsyncTaskService {
        std::unordered_map<std::string, std::shared_ptr<AsyncTask> > tasks;
        std::vector<std::shared_ptr<AsyncTask> > tasksArray;
        mutable std::mutex tasksMutex;

    public:
        std::string registerTask(const std::string &name, std::function<void()> cancel);

        void endTask(std::string taskId, bool cancel = false);

        std::vector<std::shared_ptr<AsyncTask> > getActiveTasks();

        bool hasActiveTasks();

        void endAll();
    };
}

#endif
