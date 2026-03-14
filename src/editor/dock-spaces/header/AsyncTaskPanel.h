#ifndef METAL_ENGINE_ASYNCTASKPANEL_H
#define METAL_ENGINE_ASYNCTASKPANEL_H

#include "../../abstract/AbstractPanel.h"

namespace Metal {
    class AsyncTaskService;

    class AsyncTaskPanel final : public AbstractPanel {
        AsyncTaskService *asyncTaskService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"AsyncTaskService", asyncTaskService}
            };
        }

        void onSync() override;
    };
}

#endif
