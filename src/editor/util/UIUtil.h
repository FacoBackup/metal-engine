#ifndef METAL_ENGINE_UIUTIL_H
#define METAL_ENGINE_UIUTIL_H

#include "imgui.h"
#include <string>
#include "../enum/EntryType.h"

#include "../../common/Icons.h"

namespace Metal::UIUtil {
    inline constexpr ImVec4 DIRECTORY_COLOR{188 / 255.f, 128 / 255.f, 78 / 255.f, 1};
    inline constexpr ImVec2 MEDIUM_SPACING(5, 0);
    inline constexpr ImVec2 LARGE_SPACING(40, 0);
    inline constexpr ImVec2 VEC2_ZERO(0.0f, 0.0f);
    inline constexpr int ONLY_ICON_BUTTON_SIZE = 23;
    extern bool OPEN;
    inline constexpr ImVec2 DEFAULT_PADDING(4, 4);

    bool RenderOption(const std::string &label, const bool selected, const float sizeX, const float sizeY,
                      const ImVec4 &accent);

    bool ButtonSimple(const std::string &label, const float sizeX, const float sizeY);

    bool RenderOption(const std::string &label, const bool selected, const bool fixedSize,
                      const ImVec4 &accent);

    void RenderTooltip(const std::string &text);

    void LargeSpacing();

    void Spacing();

    void DynamicSpacing(float size);

    bool DragFloatWithLabel(const std::string &id, float *value, const std::string &label, const ImVec4 &color,
                            float speed = 0.1f);

    bool DrawVec2Control(const std::string &name, const std::string &id, float *values, float speed = 0.1f);

    bool DrawVec3Control(const std::string &name, const std::string &id, float *values, float speed = 0.1f);

    bool DrawVec4Control(const std::string &name, const std::string &id, float *values, float speed = 0.1f);

    bool DrawQuatControl(const std::string &name, const std::string &id, float *values, float speed = 0.1f);

    std::string GetKeyChordName(ImGuiKeyChord keyChord);

    std::string GetEntryIcon(const EntryType::EntryType type);

    std::string GetDockSpaceIcon(const int index);
}
#endif
