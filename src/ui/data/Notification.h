#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include "../../common/enum/NotificationSeverity.h"

namespace Metal {
    struct Notification final {
        long long displayTime = -1;
        std::string message;
        NotificationSeverities::NotificationSeverity severity;

        explicit Notification(const std::string &message,
                              const NotificationSeverities::NotificationSeverity
                              severity) : message(std::move(message)), severity(severity) {
        }
    };
}
#endif //NOTIFICATION_H
