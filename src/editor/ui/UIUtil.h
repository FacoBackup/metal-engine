#ifndef METAL_ENGINE_UIUTIL_H
#define METAL_ENGINE_UIUTIL_H

#include "imgui.h"
#include <string>

#define LARGE_FONT_SIZE 38

namespace Metal::UIUtil {
    inline constexpr ImVec4 DIRECTORY_COLOR{188 / 255.f, 128 / 255.f, 78 / 255.f, 1};
    inline constexpr ImVec2 MEDIUM_SPACING(5, 0);
    inline constexpr ImVec2 LARGE_SPACING(40, 0);
    inline constexpr ImVec2 VEC2_ZERO(0.0f, 0.0f);
    inline constexpr int ONLY_ICON_BUTTON_SIZE = 23;
    extern bool OPEN;
    inline constexpr ImVec2 DEFAULT_PADDING(4, 4);

    inline constexpr float POPUP_MIN_WIDTH = 200.0f;
    inline constexpr float POPUP_ROUNDING = 8.0f;
    inline constexpr float POPUP_PADDING_Y = 6.0f;

    bool BeginPopupContext(const std::string &id);

    void EndPopupContext();

    bool RenderOption(const std::string &label, bool selected, float sizeX, float sizeY,
                      const ImVec4 &accent);

    bool RenderButtonSolid(const std::string &id, const std::string &icon, float size, const ImVec4 &color,
                           float rounding);

    bool RenderButtonSolid(const std::string &id, const std::string &icon, const ImVec2 &size, const ImVec4 &color,
                           float rounding);

    bool ButtonSimple(const std::string &label, float sizeX, float sizeY);

    bool RenderOption(const std::string &label, bool selected, bool fixedSize,
                      const ImVec4 &accent);

    bool RenderTab(const std::string &id, const std::string &icon, const std::string &label, bool selected,
                   const ImVec4 &iconColor, const ImVec4 &accent, float height);

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

    std::string GetDockSpaceIcon(int index);

    bool Accordion(const std::string &id, const std::string &label, bool &open, const ImVec4 &background);

    void EndAccordion();
}
#endif
