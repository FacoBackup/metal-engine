#ifndef METAL_ENGINE_NOTIFICATIONSPANEL_H
#define METAL_ENGINE_NOTIFICATIONSPANEL_H
#include "../abstract/AbstractPanel.h"

namespace Metal {
    class NotificationsPanel final : public AbstractPanel {
    public:
        void onSync() override;
    };
} // Metal

#endif //METAL_ENGINE_NOTIFICATIONSPANEL_H
