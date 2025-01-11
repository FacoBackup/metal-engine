#ifndef METAL_ENGINE_UIUTIL_H
#define METAL_ENGINE_UIUTIL_H

#include "imgui.h"
#include <string>
#include "../enum/EntryType.h"

#include "../common/interface/Icons.h"

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
}
#endif
