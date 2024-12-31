#ifndef NOTIFICATIONSEVERITY_H
#define NOTIFICATIONSEVERITY_H
#include <imgui.h>

#include "../common/interface/Icons.h"

namespace Metal::NotificationSeverities {
    enum NotificationSeverity {
        WARNING,
        ERROR,
        SUCCESS
    };

    static const char *GetIcon(NotificationSeverity severity) {
        switch (severity) {
            case WARNING:
                return Icons::warning.c_str();
            case ERROR:
                return Icons::error.c_str();
            case SUCCESS:
                return Icons::check.c_str();
        }
        return nullptr;
    }

    static ImVec4 GetColor(NotificationSeverity severity) {
        switch (severity) {
            case WARNING:
                return ImColor(180, 180, 0, 255);
            case ERROR:
                return ImColor(180, 0, 0, 255);
            case SUCCESS:
                return ImColor(0, 180, 0, 255);
        }
        return ImColor(255, 255, 255, 255);
    }
}

#endif //NOTIFICATIONSEVERITY_H
