#include "NotificationsPanel.h"
#include <imgui.h>
#include "../../ApplicationContext.h"
#include "../../../util/UIUtil.h"
#include "../../../dto/Notification.h"

namespace Metal {
    void NotificationsPanel::onSync() {
        int usedIndices = 0;
        for (int i = 0; i < CTX.notificationService.getNotifications().size(); i++) {
            auto *notification = CTX.notificationService.getNotifications()[i];
            if (notification == nullptr) {
                continue;
            }
            if (notification->displayTime < 0) {
                notification->displayTime = CTX.engineContext.currentTimeMs;
            }
            if (CTX.engineContext.currentTimeMs - notification->displayTime > MESSAGE_DURATION) {
                delete notification;
                CTX.notificationService.getNotifications()[i] = nullptr;
                continue;
            }
            ImGui::SetNextWindowPos(ImVec2(5, ImGui::GetMainViewport()->Size.y - 40 * (usedIndices + 1)));
            ImGui::SetNextWindowSize(ImVec2(ImGui::CalcTextSize(notification->message.c_str()).x + 45, 35));
            ImGui::PushStyleColor(ImGuiCol_WindowBg, NotificationSeverities::GetColor(notification->severity));

            ImGui::Begin("##toaster" + usedIndices, &UIUtil::OPEN,
                         ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
            ImGui::PopStyleColor();
            ImGui::TextColored(ImColor(255, 255, 255, 255), NotificationSeverities::GetIcon(notification->severity));
            ImGui::SameLine();
            ImGui::TextColored(ImColor(255, 255, 255, 255), notification->message.c_str());
            ImGui::End();

            usedIndices++;
        }
    }
} // Metal
