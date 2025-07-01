#ifndef NOTIFICATIONPANEL_H
#define NOTIFICATIONPANEL_H
#include "../../abstract/AbstractPanel.h"

namespace Metal {
    class NotificationPanel final : public AbstractPanel {
    public:
        void onSync() override;
    };
} // Metal

#endif //NOTIFICATIONPANEL_H