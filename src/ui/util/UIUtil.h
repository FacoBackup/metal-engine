#ifndef METAL_ENGINE_UIUTIL_H
#define METAL_ENGINE_UIUTIL_H

#include "imgui.h"
#include <string>
#include "../../common/enum/EntryType.h"

#include "../../common/enum/Icons.h"

namespace Metal::UIUtil {
    static constexpr ImVec4 DIRECTORY_COLOR{188 / 255.f, 128 / 255.f, 78 / 255.f, 1};
    static constexpr ImVec2 MEDIUM_SPACING(5, 0);
    static constexpr ImVec2 LARGE_SPACING(40, 0);
    static constexpr ImVec2 VEC2_ZERO(0.0f, 0.0f);
    static ImVec2 AUX_VEC2(0, 0);
    static constexpr int ONLY_ICON_BUTTON_SIZE = 23;
    static bool OPEN = true;
    static ImVec2 DEFAULT_PADDING(4, 4);

    static bool RenderOption(const std::string &label, const bool selected, const float sizeX, const float sizeY,
                             const ImVec4 &accent) {
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

    static bool ButtonSimple(const std::string &label, const float sizeX, const float sizeY) {
        AUX_VEC2.x = sizeX;
        AUX_VEC2.y = sizeY;
        return ImGui::Button(label.c_str(), AUX_VEC2);
    }

    static bool RenderOption(const std::string &label, const bool selected, const bool fixedSize,
                             const ImVec4 &accent) {
        const float size = fixedSize ? ONLY_ICON_BUTTON_SIZE : -1;
        return RenderOption(label, selected, size, size, accent);
    }

    static void RenderTooltip(const std::string &text) {
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text(text.c_str());
            ImGui::EndTooltip();
        }
    }

    static void LargeSpacing() {
        ImGui::SameLine();
        ImGui::Dummy(LARGE_SPACING);
        ImGui::SameLine();
    }

    static void Spacing() {
        ImGui::SameLine();
        ImGui::Dummy(MEDIUM_SPACING);
        ImGui::SameLine();
    }

    static void DynamicSpacing(float size) {
        ImGui::SameLine();
        AUX_VEC2.x = ImGui::GetContentRegionAvail().x - size;
        AUX_VEC2.y = 0;
        ImGui::Dummy(AUX_VEC2);
        ImGui::SameLine();
    }

    static std::string GetFileIcon(const EntryType::EntryType type) {
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
            default: return "";
        }
    }

    inline ImVec2 Add(const ImVec2 &a, const ImVec2 &b) {
        return ImVec2(a.x + b.x, a.y + b.y);
    }

    inline ImVec2 Sub(const ImVec2 &a, const ImVec2 &b) {
        return ImVec2(a.x - b.x, a.y - b.y);
    }

    static void BeginBlurChild(const char *id, ImVec2 size, float radius = 8.0f) {
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetCursorScreenPos(); // Inherit layout position

        // Draw shadow (bottom-right offset)
        ImVec2 shadow_offset = ImVec2(4, 4);
        ImU32 shadow_color = IM_COL32(0, 0, 0, 80);
        draw_list->AddRectFilled(Add(pos, shadow_offset), Add(Add(pos, size), shadow_offset), shadow_color, radius);

        // 🔷 Simulated blur: use a semi-transparent color as placeholder
        ImU32 bg_color = IM_COL32(30, 30, 30, 200); // Fake glassy look

        // Draw rounded background
        draw_list->AddRectFilled(pos, Add(pos, size), bg_color, radius);

        // Setup ImGui child window
        ImGui::SetCursorScreenPos(pos);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, radius);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0)); // Transparent so background shows through
        ImGui::BeginChild(id, size, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    }

    static void EndBlurChild() {
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }
}
#endif
