#include "NotificationPanel.h"

#include <imgui.h>
#include "../../../../MetalContextProvider.h"
#include "../../../util/UIUtil.h"
#include "../../../data/Notification.h"

#define NOTIFICATION_WIDTH  300.0f
#define NOTIFICATION_HEIGHT  35.0f
#define PADDING  10.0f

namespace Metal {
    void NotificationPanel::onSync() {
        int usedIndices = 0;
        for (int i = 0; i < SINGLETONS.notificationService.getNotifications().size(); i++) {
            auto *notification = SINGLETONS.notificationService.getNotifications()[i];
            if (notification == nullptr) {
                continue;
            }
            if (notification->displayTime < 0) {
                notification->displayTime = SINGLETONS.engineContext.currentTimeMs;
            }
            if (SINGLETONS.engineContext.currentTimeMs - notification->displayTime > MESSAGE_DURATION) {
                delete notification;
                SINGLETONS.notificationService.getNotifications()[i] = nullptr;
                continue;
            }
            ImGui::SetNextWindowPos(ImVec2(5, ImGui::GetMainViewport()->Size.y - 40 * (usedIndices + 1)));
            ImGui::SetNextWindowSize(ImVec2(ImGui::CalcTextSize(notification->message.c_str()).x + 45, 35));
            ImGui::PushStyleColor(ImGuiCol_WindowBg, GetColor(notification->severity));

            ImGui::Begin("##toaster" + usedIndices, &UIUtil::OPEN,
                         ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
            ImGui::PopStyleColor();
            ImGui::TextColored(ImColor(255, 255, 255, 255), GetIcon(notification->severity));
            ImGui::SameLine();
            ImGui::TextColored(ImColor(255, 255, 255, 255), notification->message.c_str());
            ImGui::End();

            usedIndices++;
        }
    }
} // Metal
