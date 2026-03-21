#include "ThemeService.h"

#include <glm/ext/quaternion_geometric.hpp>
#include <fstream>
#include <filesystem>
#include <iostream>

#include "../repository/EditorRepository.h"

namespace Metal {
    glm::vec3 ThemeService::BACKGROUND_COLOR = glm::vec3(0.f);

    void ThemeService::loadTheme() {
        if (themeLoaded) return;

        std::string filename = "theme.json";
        if (!std::filesystem::exists(filename)) {
            filename = "resources/theme.json";
        }

        if (std::filesystem::exists(filename)) {
            try {
                std::ifstream f(filename);
                themeData = nlohmann::json::parse(f);
                themeLoaded = true;
            } catch (const std::exception& e) {
                std::cerr << "Failed to parse theme.json: " << e.what() << std::endl;
            }
        } else {
            std::cerr << "theme.json not found in root or resources directory." << std::endl;
        }
    }

    ImVec4 ThemeService::hexToRGBA(const std::string& hex) {
        std::string h = hex;
        if (h[0] == '#') h = h.substr(1);

        if (h.length() == 6) {
            int r = std::stoi(h.substr(0, 2), nullptr, 16);
            int g = std::stoi(h.substr(2, 2), nullptr, 16);
            int b = std::stoi(h.substr(4, 2), nullptr, 16);
            return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
        } else if (h.length() == 8) {
            int r = std::stoi(h.substr(0, 2), nullptr, 16);
            int g = std::stoi(h.substr(2, 2), nullptr, 16);
            int b = std::stoi(h.substr(4, 2), nullptr, 16);
            int a = std::stoi(h.substr(6, 2), nullptr, 16);
            return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
        }
        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Default white
    }

    void ThemeService::onSync() {
        loadTheme();
        if (previousTheme == editorRepository->isDarkMode && glm::length(editorRepository->accentColor) == prevLength) {
            return;
        }
        prevLength = glm::length(editorRepository->accentColor);
        previousTheme = editorRepository->isDarkMode;

        ImGuiStyle &style = ImGui::GetStyle();
        auto &colors = style.Colors;

        if (!editorRepository->isDarkMode) {
            ImGui::StyleColorsLight();
            setLightMode();
        } else {
            ImGui::StyleColorsDark();
            setDarkMode();
        }

        textDisabled = ImVec4(palette6.x / 2.f, palette6.y / 2.f, palette6.z / 2.f, 1);
        colors[ImGuiCol_Text] = palette6;
        colors[ImGuiCol_TextDisabled] = textDisabled;
        colors[ImGuiCol_WindowBg] = ImVec4(0, 0, 0, 0);
        colors[ImGuiCol_ChildBg] = palette1;
        colors[ImGuiCol_PopupBg] = palette1;
        colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 0);
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
        colors[ImGuiCol_Separator] = palette1;
        colors[ImGuiCol_ResizeGrip] = palette1;
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

        editorRepository->accent.y = editorRepository->accentColor.y;
        editorRepository->accent.z = editorRepository->accentColor.z;
        editorRepository->accent.x = editorRepository->accentColor.x;
        editorRepository->accent.w = 1;
        editorRepository->accentU32 = ImGui::GetColorU32(editorRepository->accent);

        colors[ImGuiCol_FrameBgHovered] = editorRepository->accent;
        colors[ImGuiCol_FrameBgActive] = editorRepository->accent;
        colors[ImGuiCol_CheckMark] = editorRepository->accent;
        colors[ImGuiCol_SliderGrabActive] = editorRepository->accent;
        colors[ImGuiCol_Button] = palette3;
        colors[ImGuiCol_ButtonHovered] = editorRepository->accent;
        colors[ImGuiCol_Header] = editorRepository->accent;
        colors[ImGuiCol_HeaderHovered] = editorRepository->accent;
        colors[ImGuiCol_HeaderActive] = editorRepository->accent;
        colors[ImGuiCol_ResizeGripHovered] = editorRepository->accent;
        colors[ImGuiCol_ResizeGripActive] = editorRepository->accent;
        colors[ImGuiCol_TextSelectedBg] = editorRepository->accent;

        BACKGROUND_COLOR[0] = colors[ImGuiCol_WindowBg].x;
        BACKGROUND_COLOR[1] = colors[ImGuiCol_WindowBg].y;
        BACKGROUND_COLOR[2] = colors[ImGuiCol_WindowBg].z;
    }

    void ThemeService::setDarkMode() {
        if (themeLoaded && themeData.contains("dark")) {
            auto& dark = themeData["dark"];
            palette0 = hexToRGBA(dark.value("palette0", "#121212"));
            palette1 = hexToRGBA(dark.value("palette1", "#0A0A0A"));
            palette2 = hexToRGBA(dark.value("palette2", "#161616"));
            palette3 = hexToRGBA(dark.value("palette3", "#232323"));
            palette4 = hexToRGBA(dark.value("palette4", "#414141"));
            palette5 = hexToRGBA(dark.value("palette5", "#777777"));
            palette6 = hexToRGBA(dark.value("palette6", "#E0E0E0"));
            neutralPalette = hexToRGBA(dark.value("neutralPalette", "#161616"));
        }
    }

    void ThemeService::setLightMode() {
        if (themeLoaded && themeData.contains("light")) {
            auto& light = themeData["light"];
            palette0 = hexToRGBA(light.value("palette0", "#EBEBEB"));
            palette1 = hexToRGBA(light.value("palette1", "#F5F5F5"));
            palette2 = hexToRGBA(light.value("palette2", "#E1E1E1"));
            palette3 = hexToRGBA(light.value("palette3", "#C8C8C8"));
            palette4 = hexToRGBA(light.value("palette4", "#A0A0A0"));
            palette5 = hexToRGBA(light.value("palette5", "#787878"));
            palette6 = hexToRGBA(light.value("palette6", "#0A0A0A"));
            neutralPalette = hexToRGBA(light.value("neutralPalette", "#E1E1E1"));
        }
    }
} // Metal
