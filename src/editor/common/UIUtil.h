#ifndef METAL_ENGINE_UIUTIL_H
#define METAL_ENGINE_UIUTIL_H

#include "imgui.h"
#include <string>

namespace Metal::UIUtil {
    static const int FIXED_WINDOW_FLAGS =
            ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
    static const ImVec2 MEDIUM_SPACING(5, 0);
    static const ImVec2 LARGE_SPACING(40, 0);
    static const ImVec2 VEC2_ZERO(0.0f, 0.0f);
    static ImVec2 AUX_VEC2(0, 0);
    static const int ONLY_ICON_BUTTON_SIZE = 23;
    static bool OPEN = true;

    static bool RenderOption(const std::string &label, bool selected, float sizeX, float sizeY, ImVec4 accent) {
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

    static bool ButtonSimple(const std::string &label, float sizeX, float sizeY) {
        AUX_VEC2.x = sizeX;
        AUX_VEC2.y = sizeY;
        return ImGui::Button(label.c_str(), AUX_VEC2);
    }

    static bool RenderOption(const std::string &label, bool selected, bool fixedSize, ImVec4 accent) {
        float size = fixedSize ? ONLY_ICON_BUTTON_SIZE : -1;
        return RenderOption(label, selected, size, size, accent);
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
}
#endif
