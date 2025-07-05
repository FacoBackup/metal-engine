#include "UIUtil.h"
#include "../../MetalContextProvider.h"

namespace Metal::UIUtil {
    const ImVec4 DIRECTORY_COLOR{188 / 255.f, 128 / 255.f, 78 / 255.f, 1};
    const ImVec2 MEDIUM_SPACING(5, 0);
    const ImVec2 LARGE_SPACING(40, 0);
    const ImVec2 VEC2_ZERO(0.0f, 0.0f);
    const int ONLY_ICON_BUTTON_SIZE = 23;

    bool OPEN = true;
    ImVec2 AUX_VEC2(0, 0);
    ImVec2 DEFAULT_PADDING(4, 4);

    bool RenderOption(const std::string& label, bool selected, float sizeX, float sizeY, const ImVec4& accent) {
        int popStyle = 0;
        if (selected) {
            ImGui::PushStyleColor(ImGuiCol_Button, accent);
            popStyle++;
        }

        bool value;
        if (sizeX == -1) {
            value = ImGui::Button(label.c_str());
        } else {
            AUX_VEC2.x = sizeX;
            AUX_VEC2.y = sizeY;
            value = ImGui::Button(label.c_str(), AUX_VEC2);
        }

        ImGui::PopStyleColor(popStyle);
        return value;
    }

    bool ButtonSimple(const std::string& label, float sizeX, float sizeY) {
        AUX_VEC2.x = sizeX;
        AUX_VEC2.y = sizeY;
        return ImGui::Button(label.c_str(), AUX_VEC2);
    }

    bool RenderOption(const std::string& label, bool selected, bool fixedSize, const ImVec4& accent) {
        const float size = fixedSize ? ONLY_ICON_BUTTON_SIZE : -1;
        return RenderOption(label, selected, size, size, accent);
    }

    void RenderTooltip(const std::string& text) {
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", text.c_str());
            ImGui::EndTooltip();
        }
    }

    void LargeSpacing() {
        ImGui::SameLine();
        ImGui::Dummy(LARGE_SPACING);
        ImGui::SameLine();
    }

    void Spacing() {
        ImGui::SameLine();
        ImGui::Dummy(MEDIUM_SPACING);
        ImGui::SameLine();
    }

    void DynamicSpacing(float size) {
        ImGui::SameLine();
        AUX_VEC2.x = ImGui::GetContentRegionAvail().x - size;
        AUX_VEC2.y = 0;
        ImGui::Dummy(AUX_VEC2);
        ImGui::SameLine();
    }

    std::string GetFileIcon(EntryType::EntryType type) {
        switch (type) {
            case EntryType::MESH:
                return Icons::view_in_ar;
            case EntryType::TEXTURE:
                return Icons::texture;
            case EntryType::DIRECTORY:
                return Icons::folder;
            case EntryType::SCENE:
                return Icons::inventory_2;
            case EntryType::MATERIAL:
                return Icons::format_paint;
            default:
                return "";
        }
    }

    ImVec2 Add(const ImVec2& a, const ImVec2& b) {
        return ImVec2(a.x + b.x, a.y + b.y);
    }

    ImVec2 Sub(const ImVec2& a, const ImVec2& b) {
        return ImVec2(a.x - b.x, a.y - b.y);
    }

    void BeginWindow(ImVec2 pos, ImVec2 size) {
        ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(size, ImGuiCond_Always);
        SINGLETONS.windowService.beginWindow(glm::vec2(pos.x, pos.y), glm::vec2(size.x, size.y));
    }


     void BeginEmptyWindow(const char *id, ImVec2 size) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0)); // Transparent so background shows through
        ImGui::BeginChild(id, size, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    }

     void EndEmptyWindow() {
        ImGui::EndChild();
        ImGui::PopStyleColor();
    }
}
