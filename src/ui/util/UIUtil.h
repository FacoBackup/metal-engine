#ifndef METAL_ENGINE_UIUTIL_H
#define METAL_ENGINE_UIUTIL_H

#include "imgui.h"
#include <string>
#include "../../common/enum/EntryType.h"

namespace Metal::UIUtil {
    // Constants
    extern const ImVec4 DIRECTORY_COLOR;
    extern const ImVec2 MEDIUM_SPACING;
    extern const ImVec2 LARGE_SPACING;
    extern const ImVec2 VEC2_ZERO;
    extern const int ONLY_ICON_BUTTON_SIZE;

    // Globals
    extern bool OPEN;
    extern ImVec2 AUX_VEC2;
    extern ImVec2 DEFAULT_PADDING;

    // Function declarations
    bool RenderOption(const std::string &label, bool selected, float sizeX, float sizeY, const ImVec4 &accent);

    bool RenderOption(const std::string &label, bool selected, bool fixedSize, const ImVec4 &accent);

    bool ButtonSimple(const std::string &label, float sizeX, float sizeY);

    void RenderTooltip(const std::string &text);

    void LargeSpacing();

    void Spacing();

    void DynamicSpacing(float size);

    std::string GetFileIcon(EntryType::EntryType type);

    ImVec2 Add(const ImVec2 &a, const ImVec2 &b);

    ImVec2 Sub(const ImVec2 &a, const ImVec2 &b);

    void BeginWindow(ImVec2 pos, ImVec2 size);

    void BeginEmptyWindow(const char *id, ImVec2 size);

    void EndEmptyWindow();

    void drawSplitter(const char *id, float &size,  bool isHorizontal);
}
#endif
