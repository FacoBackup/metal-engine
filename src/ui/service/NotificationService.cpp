#include "NotificationService.h"
#include "../data/Notification.h"

namespace Metal {
    void NotificationService::pushMessage(const std::string &message, NotificationSeverities::NotificationSeverity severity) {
        pushMessage(new Notification(message, severity));
    }

    void NotificationService::pushMessage(Notification *message) {
        if (message == nullptr) {
            return;
        }

        if (notifications[0] == nullptr) {
            notifications[0] = message;
        } else if (notifications[1] == nullptr) {
            notifications[1] = message;
        } else if (notifications[2] == nullptr) {
            notifications[2] = message;
        } else {
            notifications[3] = message;
        }
    }

    std::array<Notification *, 4> & NotificationService::getNotifications() {
        return notifications;
    }
} // Metal
