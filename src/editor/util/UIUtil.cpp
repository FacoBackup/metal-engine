#include "UIUtil.h"
#include "../../common/Icons.h"
#include "../../common/FileExtensions.h"

namespace Metal::UIUtil {
    bool OPEN = true;

    bool BeginPopupContext(const std::string &id) {
        ImGui::SetNextWindowSizeConstraints(ImVec2(POPUP_MIN_WIDTH, 0), ImVec2(FLT_MAX, FLT_MAX));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, POPUP_ROUNDING);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, POPUP_PADDING_Y));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, POPUP_PADDING_Y));

        if (ImGui::BeginPopup(id.c_str())) {
            return true;
        }

        ImGui::PopStyleVar(3);
        return false;
    }

    void EndPopupContext() {
        ImGui::EndPopup();
        ImGui::PopStyleVar(3);
    }

    bool RenderOption(const std::string &label, const bool selected, const float sizeX, const float sizeY,
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
            value = ImGui::Button(label.c_str(), ImVec2(sizeX, sizeY));
        }

        ImGui::PopStyleColor(popStyle);
        return value;
    }

    bool ButtonSimple(const std::string &label, const float sizeX, const float sizeY) {
        return ImGui::Button(label.c_str(), ImVec2(sizeX, sizeY));
    }

    bool RenderOption(const std::string &label, const bool selected, const bool fixedSize,
                      const ImVec4 &accent) {
        const float size = fixedSize ? ONLY_ICON_BUTTON_SIZE : -1;
        return RenderOption(label, selected, size, size, accent);
    }

    void RenderTooltip(const std::string &text) {
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text(text.c_str());
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
        ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x - size, 0));
        ImGui::SameLine();
    }

    bool DragFloatWithLabel(const std::string &id, float *value, const std::string &label, const ImVec4 &color,
                            float speed) {
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

    bool DrawVec2Control(const std::string &name, const std::string &id, float *values, float speed) {
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

    bool DrawVec3Control(const std::string &name, const std::string &id, float *values, float speed) {
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

    bool DrawVec4Control(const std::string &name, const std::string &id, float *values, float speed) {
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

    bool DrawQuatControl(const std::string &name, const std::string &id, float *values, float speed) {
        return DrawVec4Control(name, id, values, speed);
    }

    std::string GetKeyChordName(ImGuiKeyChord keyChord) {
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

    std::string GetDockSpaceIcon(const int index) {
        switch (index) {
            case -1: return Icons::i_public; // VIEWPORT
            case 0: return Icons::search; // INSPECTOR
            case 1: return Icons::account_tree; // WORLD
            case 2: return Icons::terminal; // CONSOLE
            case 3: return Icons::folder_open; // FILES
            case 4: return Icons::analytics; // METRICS
            case 5: return Icons::search; // REPOSITORIES
            default: return "";
        }
    }

    std::string GetExtensionIcon(const std::string &extension) {
        if (extension == EXT_SCENE) return Icons::image;
        if (extension == EXT_MESH) return Icons::view_in_ar;
        if (extension == EXT_PNG || extension == EXT_JPG || extension == EXT_JPEG || extension == EXT_TGA) return Icons::texture;
        if (extension == EXT_SVO || extension == EXT_VDB) return Icons::view_agenda;
        if (extension == EXT_OBJ || extension == EXT_FBX || extension == EXT_GLTF || extension == EXT_GLB) return Icons::image;
        return Icons::close;
    }

    std::string GetExtensionLabel(const std::string &extension) {
        if (extension == EXT_SCENE) return "Scene";
        if (extension == EXT_MESH) return "Mesh";
        if (extension == EXT_PNG || extension == EXT_JPG || extension == EXT_JPEG || extension == EXT_TGA) return "Texture";
        if (extension == EXT_SVO) return "Volume";
        if (extension == EXT_VDB) return "OpenVDB";
        if (extension == EXT_OBJ || extension == EXT_FBX || extension == EXT_GLTF || extension == EXT_GLB) return "Scene Source";
        return "File";
    }

    bool Accordion(const std::string &id, const std::string &label, bool &open, const ImVec4 &background) {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, open ? ImVec2(4.0f, 4.0f) : ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, background);
        if (ImGui::BeginChild(id.c_str(), ImVec2(0, 0),
                              ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding)) {
            ImGui::PushStyleColor(ImGuiCol_Button, background);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

            const std::string icon = open ? Icons::keyboard_arrow_down : Icons::keyboard_arrow_right;
            if (ImGui::Button((icon + " " + label + "##" + id).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                open = !open;
            }

            ImGui::PopStyleVar(3);
            ImGui::PopStyleColor();

            if (open) {
                return true;
            }
        }

        EndAccordion();
        return false;
    }

    void EndAccordion() {
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }
}
