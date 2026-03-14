#include "AsyncTaskPanel.h"
#include "../../../ApplicationContext.h"
#include "../../util/UIUtil.h"
#include <cmath>
#include "../../service/AsyncTaskService.h"
#include "../../dto/AsyncTask.h"

namespace Metal {
    void AsyncTaskPanel::onSync() {
        auto tasks = asyncTaskService->getActiveTasks();
        if (tasks.empty()) {
            UIUtil::DynamicSpacing(90);
            return;
        }
        UIUtil::DynamicSpacing(250);
        ImGui::SameLine();

        float time = (float)ImGui::GetTime();
        float t = 0.5f + 0.5f * std::sin(time * 3.0f); // 0.0 to 1.0

        ImVec4 color = ImVec4(
            0.0f + t * 0.4f,
            0.2f + t * 0.5f,
            0.6f + t * 0.4f,
            1.0f
        );

        ImVec2 pos = ImGui::GetCursorScreenPos();
        float width = 150.0f;
        float height = 10.0f;

        pos.y += (UIUtil::ONLY_ICON_BUTTON_SIZE - height) / 2.0f;

        ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), ImGui::ColorConvertFloat4ToU32(color), 5.0f);

        ImGui::SetCursorScreenPos(pos);
        if (ImGui::InvisibleButton("##AsyncTaskLine", ImVec2(width, height))) {
            ImGui::OpenPopup("AsyncTasksPopup");
        }
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(0, UIUtil::ONLY_ICON_BUTTON_SIZE));

        UIUtil::RenderTooltip(Icons::sync + " " + std::to_string(tasks.size()) + " processes running...");

        float windowWidth = ImGui::GetIO().DisplaySize.x;
        ImGui::SetNextWindowSize(ImVec2(windowWidth * 0.25f, 0));
        if (ImGui::BeginPopup("AsyncTasksPopup")) {
            ImGui::Text("Background Processes");
            ImGui::Separator();
            for (std::shared_ptr<AsyncTask> &task : tasks) {
                ImGui::BeginGroup();
                ImGui::Text("%s", task->name.c_str());

                ImVec2 p = ImGui::GetCursorScreenPos();
                float w = ImGui::GetContentRegionAvail().x - UIUtil::ONLY_ICON_BUTTON_SIZE - ImGui::GetStyle().ItemSpacing.x;
                ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + w, p.y + height), ImGui::ColorConvertFloat4ToU32(color), 5.0f);
                ImGui::Dummy(ImVec2(w, height));

                ImGui::SameLine();
                if (ImGui::Button((Icons::close + "##" + task->id).c_str(), ImVec2(UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE))) {
                    asyncTaskService->endTask(task->id, true);
                }
                ImGui::EndGroup();
            }
            ImGui::EndPopup();
        }
    }
}
