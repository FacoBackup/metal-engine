#include "AsyncTaskService.h"
#include <algorithm>
#include <utility>
#include "../util/Util.h"
#include "../dto/AsyncTask.h"

namespace Metal {
    std::string AsyncTaskService::registerTask(const std::string &name, std::function<void()> cancel) {
        std::lock_guard<std::mutex> lock(tasksMutex);
        std::shared_ptr<AsyncTask> task = std::make_shared<AsyncTask>();
        task->name = name;
        task->progress = -1.f;
        task->completed = false;
        task->id = Util::uuidV4();
        task->cancel = std::move(cancel);
        tasksArray.push_back(task);
        tasks.emplace(task->id, task);
        return task->id;
    }

    void AsyncTaskService::endTask(std::string taskId, bool cancel) {
        std::shared_ptr<AsyncTask> task = tasks.contains(taskId) ? tasks.at(taskId) : nullptr;
        if (task == nullptr) return;
        if (cancel) {
            task->cancel();
        }
        task->completed = true;
        std::lock_guard<std::mutex> lock(tasksMutex);
        tasks.erase(taskId);
        std::erase(tasksArray, task);
    }

    std::vector<std::shared_ptr<AsyncTask> > AsyncTaskService::getActiveTasks() {
        std::lock_guard<std::mutex> lock(tasksMutex);
        return tasksArray;
    }

    bool AsyncTaskService::hasActiveTasks() {
        std::lock_guard<std::mutex> lock(tasksMutex);
        return !tasks.empty();
    }

    void AsyncTaskService::endAll() {
        for (std::shared_ptr<AsyncTask> &task: tasksArray) {
            endTask(task->id, true);
        }
    }
}
