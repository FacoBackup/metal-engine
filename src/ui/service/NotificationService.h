#ifndef TOASTERSERVICE_H
#define TOASTERSERVICE_H
#include <string>
#include <array>
#include "../../common/enum/NotificationSeverity.h"

#define MAX_notifications  4
#define  MESSAGE_DURATION  3000

namespace Metal {
    struct Notification;

    class NotificationService {
        std::array<Notification *, MAX_notifications> notifications{};

    public:
        void pushMessage(const std::string &message, NotificationSeverities::NotificationSeverity severity);

        void pushMessage(Notification *message);

        std::array<Notification *, MAX_notifications> &getNotifications();
    };
} // Metal

#endif //TOASTERSERVICE_H
