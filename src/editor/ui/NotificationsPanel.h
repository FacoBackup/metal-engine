#ifndef METAL_ENGINE_NOTIFICATIONSPANEL_H
#define METAL_ENGINE_NOTIFICATIONSPANEL_H
#include "editor/ui/abstract/AbstractPanel.h"

namespace Metal {
    class NotificationService;
    class EngineContext;

    class NotificationsPanel final : public AbstractPanel {
        NotificationService *notificationService = nullptr;
        EngineContext *engineContext = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"NotificationService", &notificationService},
                {"EngineContext", &engineContext}
            };
        }

        void onSync() override;
    };
} // Metal

#endif //METAL_ENGINE_NOTIFICATIONSPANEL_H
