#include "AsyncTaskPanel.h"
#include "../../../ApplicationContext.h"
#include "../../util/UIUtil.h"
#include <cmath>
#include "../../service/AsyncTaskService.h"
#include "../../dto/AsyncTask.h"

namespace Metal {
    void AsyncTaskPanel::onSync() {
        auto tasks = asyncTaskService->getActiveTasks();
        bool hasTasks = !tasks.empty();

        ImGui::BeginGroup();
        ImVec2 pos = ImGui::GetCursorScreenPos();
        float size = (float)UIUtil::ONLY_ICON_BUTTON_SIZE;

        if (ImGui::InvisibleButton("##AsyncTaskIcon", ImVec2(size, size))) {
            ImGui::OpenPopup("AsyncTasksPopup");
        }

        ImU32 iconColor = ImGui::GetColorU32(ImGuiCol_Text);
        ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(),
                                            ImVec2(pos.x + (size - ImGui::CalcTextSize(Icons::sync.c_str()).x) / 2,
                                                   pos.y + (size - ImGui::GetFontSize()) / 2),
                                            iconColor, Icons::sync.c_str());

        if (hasTasks) {
            float circleRadius = 3.0f;
            ImVec2 circlePos = ImVec2(pos.x + size / 2.0f, pos.y + size - circleRadius - 2.0f);
            ImGui::GetWindowDrawList()->AddCircleFilled(circlePos, circleRadius, IM_COL32(0, 200, 255, 255));
        }
        ImGui::EndGroup();

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
                float height = 10.0f;
                float w = ImGui::GetContentRegionAvail().x - UIUtil::ONLY_ICON_BUTTON_SIZE - ImGui::GetStyle().ItemSpacing.x;
                ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + w, p.y + height), IM_COL32(50, 150, 255, 255), 5.0f);
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
