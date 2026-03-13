#ifndef METAL_ENGINE_UIUTIL_H
#define METAL_ENGINE_UIUTIL_H

#include "imgui.h"
#include <string>
#include "../enum/EntryType.h"

#include "../../../common/Icons.h"

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

    static bool DragFloatWithLabel(const std::string &id, float *value, const std::string &label, const ImVec4 &color,
                                   float speed = 0.1f) {
        bool changed = false;
        ImGui::PushID(id.c_str());

        const float framePadding = ImGui::GetStyle().FramePadding.x;
        const float labelWidth = ImGui::CalcTextSize(label.c_str()).x + framePadding * 2.0f;

        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
        ImGui::Button(label.c_str(), ImVec2(labelWidth, 0));
        ImGui::PopStyleColor(3);

        ImGui::SameLine(0, 0);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        changed = ImGui::DragFloat("##v", value, speed);

        ImGui::PopID();
        return changed;
    }

    static bool DrawVec2Control(const std::string &name, const std::string &id, float *values, float speed = 0.1f) {
        bool changed = false;
        ImGui::Text(name.c_str());
        ImGui::PushID(id.c_str());

        if (ImGui::BeginTable("##table", 2, ImGuiTableFlags_NoSavedSettings)) {
            ImGui::TableNextColumn();
            if (DragFloatWithLabel("x", &values[0], "X", ImVec4{0.8f, 0.1f, 0.15f, 1.0f}, speed)) changed = true;
            ImGui::TableNextColumn();
            if (DragFloatWithLabel("y", &values[1], "Y", ImVec4{0.2f, 0.7f, 0.2f, 1.0f}, speed)) changed = true;
            ImGui::EndTable();
        }

        ImGui::PopID();
        return changed;
    }

    static bool DrawVec3Control(const std::string &name, const std::string &id, float *values, float speed = 0.1f) {
        bool changed = false;
        ImGui::Text(name.c_str());
        ImGui::PushID(id.c_str());

        if (ImGui::BeginTable("##table", 3, ImGuiTableFlags_NoSavedSettings)) {
            ImGui::TableNextColumn();
            if (DragFloatWithLabel("x", &values[0], "X", ImVec4{0.8f, 0.1f, 0.15f, 1.0f}, speed)) changed = true;
            ImGui::TableNextColumn();
            if (DragFloatWithLabel("y", &values[1], "Y", ImVec4{0.2f, 0.7f, 0.2f, 1.0f}, speed)) changed = true;
            ImGui::TableNextColumn();
            if (DragFloatWithLabel("z", &values[2], "Z", ImVec4{0.1f, 0.25f, 0.8f, 1.0f}, speed)) changed = true;
            ImGui::EndTable();
        }

        ImGui::PopID();
        return changed;
    }

    static bool DrawVec4Control(const std::string &name, const std::string &id, float *values, float speed = 0.1f) {
        bool changed = false;
        ImGui::Text(name.c_str());
        ImGui::PushID(id.c_str());

        if (ImGui::BeginTable("##table", 4, ImGuiTableFlags_NoSavedSettings)) {
            ImGui::TableNextColumn();
            if (DragFloatWithLabel("x", &values[0], "X", ImVec4{0.8f, 0.1f, 0.15f, 1.0f}, speed)) changed = true;
            ImGui::TableNextColumn();
            if (DragFloatWithLabel("y", &values[1], "Y", ImVec4{0.2f, 0.7f, 0.2f, 1.0f}, speed)) changed = true;
            ImGui::TableNextColumn();
            if (DragFloatWithLabel("z", &values[2], "Z", ImVec4{0.1f, 0.25f, 0.8f, 1.0f}, speed)) changed = true;
            ImGui::TableNextColumn();
            if (DragFloatWithLabel("w", &values[3], "W", ImVec4{0.5f, 0.5f, 0.5f, 1.0f}, speed)) changed = true;
            ImGui::EndTable();
        }

        ImGui::PopID();
        return changed;
    }

    static bool DrawQuatControl(const std::string &name, const std::string &id, float *values, float speed = 0.1f) {
        return DrawVec4Control(name, id, values, speed);
    }

    static std::string GetKeyChordName(ImGuiKeyChord keyChord) {
        std::string result;
        if (keyChord & ImGuiMod_Ctrl) result += "Ctrl+";
        if (keyChord & ImGuiMod_Shift) result += "Shift+";
        if (keyChord & ImGuiMod_Alt) result += "Alt+";
        if (keyChord & ImGuiMod_Super) result += "Super+";

        const ImGuiKey key = static_cast<ImGuiKey>(keyChord & ~ImGuiMod_Mask_);
        if (key != ImGuiKey_None) {
            result += ImGui::GetKeyName(key);
        } else if (!result.empty() && result.back() == '+') {
            result.pop_back();
        }
        return result;
    }

    static std::string GetEntryIcon(const EntryType::EntryType type) {
        switch (type) {
            case EntryType::MESH:
                return Icons::view_in_ar;
            case EntryType::TEXTURE:
                return Icons::texture;
            case EntryType::VOLUME:
                return Icons::cloud;
            case EntryType::DIRECTORY:
                return Icons::folder;
            case EntryType::SCENE:
                return Icons::inventory_2;
            default: return "";
        }
    }
}
#endif
