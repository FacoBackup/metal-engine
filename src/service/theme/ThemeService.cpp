#include "ThemeService.h"

#include "../../context/ApplicationContext.h"

namespace Metal {
    glm::vec3 ThemeService::BACKGROUND_COLOR = glm::vec3(0.f);

    void ThemeService::onSync() {
        EditorRepository &editorRepository = context.editorRepository;
        if (previousTheme == editorRepository.isDarkMode && glm::length(editorRepository.accentColor) == prevLength) {
            return;
        }
        prevLength = glm::length(editorRepository.accentColor);
        previousTheme = editorRepository.isDarkMode;

        ImGuiStyle &style = ImGui::GetStyle();
        auto &colors = style.Colors;

        if (!editorRepository.isDarkMode) {
            ImGui::StyleColorsLight();
            setLightMode();
        } else {
            ImGui::StyleColorsDark();
            setDarkMode();
        }

        textDisabled = ImVec4(palette6.x / 2.f, palette6.y / 2.f, palette6.z / 2.f, 1);
        colors[ImGuiCol_Text] = palette6;
        colors[ImGuiCol_TextDisabled] = textDisabled;
        colors[ImGuiCol_WindowBg] = palette1;
        colors[ImGuiCol_ChildBg] = palette1;
        colors[ImGuiCol_PopupBg] = palette1;
        colors[ImGuiCol_Border] = palette3;
        colors[ImGuiCol_BorderShadow] = ImVec4(0.f, 0.f, 0.f, 0.f);
        colors[ImGuiCol_FrameBg] = palette2;
        colors[ImGuiCol_TitleBg] = palette1;
        colors[ImGuiCol_TitleBgActive] = palette1;
        colors[ImGuiCol_TitleBgCollapsed] = palette1;
        colors[ImGuiCol_MenuBarBg] = palette0;
        colors[ImGuiCol_ScrollbarBg] = palette0;
        colors[ImGuiCol_ScrollbarGrab] = palette3;
        colors[ImGuiCol_ScrollbarGrabHovered] = palette4;
        colors[ImGuiCol_ScrollbarGrabActive] = palette2;
        colors[ImGuiCol_SliderGrab] = palette4;
        colors[ImGuiCol_ButtonActive] = palette2;
        colors[ImGuiCol_Separator] = palette5;
        colors[ImGuiCol_SeparatorHovered] = palette6;
        colors[ImGuiCol_SeparatorActive] = palette6;
        colors[ImGuiCol_ResizeGrip] = palette4;
        colors[ImGuiCol_Tab] = palette2;
        colors[ImGuiCol_TabHovered] = palette3;
        colors[ImGuiCol_DockingPreview] = palette4;
        colors[ImGuiCol_DockingEmptyBg] = palette6;
        colors[ImGuiCol_PlotLines] = palette5;
        colors[ImGuiCol_PlotLinesHovered] = palette6;
        colors[ImGuiCol_PlotHistogram] = palette5;
        colors[ImGuiCol_PlotHistogramHovered] = palette6;
        colors[ImGuiCol_DragDropTarget] = palette4;
        colors[ImGuiCol_NavHighlight] = palette3;
        colors[ImGuiCol_NavWindowingHighlight] = palette2;
        colors[ImGuiCol_NavWindowingDimBg] = palette2;
        colors[ImGuiCol_ModalWindowDimBg] = palette2;

        editorRepository.accent.y = editorRepository.accentColor.y;
        editorRepository.accent.z = editorRepository.accentColor.z;
        editorRepository.accent.x = editorRepository.accentColor.x;
        editorRepository.accent.w = 1;
        editorRepository.accentU32 = ImGui::GetColorU32(editorRepository.accent);

        colors[ImGuiCol_FrameBgHovered] = editorRepository.accent;
        colors[ImGuiCol_FrameBgActive] = editorRepository.accent;
        colors[ImGuiCol_CheckMark] = editorRepository.accent;
        colors[ImGuiCol_SliderGrabActive] = editorRepository.accent;
        colors[ImGuiCol_Button] = palette3;
        colors[ImGuiCol_ButtonHovered] = editorRepository.accent;
        colors[ImGuiCol_Header] = editorRepository.accent;
        colors[ImGuiCol_HeaderHovered] = editorRepository.accent;
        colors[ImGuiCol_HeaderActive] = editorRepository.accent;
        colors[ImGuiCol_ResizeGripHovered] = editorRepository.accent;
        colors[ImGuiCol_ResizeGripActive] = editorRepository.accent;
        colors[ImGuiCol_TextSelectedBg] = editorRepository.accent;

        BACKGROUND_COLOR[0] = colors[ImGuiCol_WindowBg].x;
        BACKGROUND_COLOR[1] = colors[ImGuiCol_WindowBg].y;
        BACKGROUND_COLOR[2] = colors[ImGuiCol_WindowBg].z;
    }

    void ThemeService::setDarkMode() {
        palette0 = ImVec4(10.0f / 255.0f, 10.0f / 255.0f, 10.0f / 255.0f, 1.0f);
        palette1 = ImVec4(18.0f / 255.0f, 18.0f / 255.0f, 18.0f / 255.0f, 1.0f);
        palette2 = ImVec4(22.0f / 255.0f, 22.0f / 255.0f, 22.0f / 255.0f, 1.0f);
        neutralPalette = palette2;
        palette3 = ImVec4(35.0f / 255.0f, 35.0f / 255.0f, 35.0f / 255.0f, 1.0f);
        palette4 = ImVec4(65.0f / 255.0f, 65.0f / 255.0f, 65.0f / 255.0f, 1.0f);
        palette5 = ImVec4(119.0f / 255.0f, 119.0f / 255.0f, 119.0f / 255.0f, 1.0f);
        palette6 = ImVec4(224.0f / 255.0f, 224.0f / 255.0f, 224.0f / 255.0f, 1.0f);
    }

    void ThemeService::setLightMode() {
        palette0 = ImVec4(245.0f / 255.0f, 245.0f / 255.0f, 245.0f / 255.0f, 1.0f); // light gray
        palette1 = ImVec4(235.0f / 255.0f, 235.0f / 255.0f, 235.0f / 255.0f, 1.0f); // slightly darker gray
        palette2 = ImVec4(225.0f / 255.0f, 225.0f / 255.0f, 225.0f / 255.0f, 1.0f); // medium gray
        neutralPalette = palette2;
        palette3 = ImVec4(200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f, 1.0f); // darker gray
        palette4 = ImVec4(160.0f / 255.0f, 160.0f / 255.0f, 160.0f / 255.0f, 1.0f); // even darker gray
        palette5 = ImVec4(120.0f / 255.0f, 120.0f / 255.0f, 120.0f / 255.0f, 1.0f); // dark gray
        palette6 = ImVec4(10.0f / 255.0f, 10.0f / 255.0f, 10.0f / 255.0f, 1.0f); // very dark gray
    }
} // Metal
