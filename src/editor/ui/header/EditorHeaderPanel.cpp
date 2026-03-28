#include "EditorHeaderPanel.h"
#include "editor/ui/UIUtil.h"
#include "../../repository/EditorRepository.h"
#include "../../../core/DirectoryService.h"
#include "../../../core/service/SnapshotService.h"
#include "../../../core/WindowService.h"
#include "engine/EngineContext.h"
#include "../../service/DockService.h"
#include "../EditorPanel.h"
#include "editor/service/HistoryService.h"
#include "editor/service/ThemeService.h"
#include "engine/service/TextureService.h"
#include "../../../core/ImGuiService.h"
#include "../../../ApplicationEventContext.h"

namespace Metal {
    void EditorHeaderPanel::onSync() {
        float windowWidth = ImGui::GetIO().DisplaySize.x;

        WindowService::WindowControlRects rects{};

        float buttonWidth = 45.0f;
        float xPos = windowWidth - buttonWidth;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
        // Close Button
        rects.closeButton = {
            static_cast<int>(xPos), 0, static_cast<int>(buttonWidth), static_cast<int>(EditorPanel::HEADER_HEIGHT)
        };
        RenderWindowControl(Icons::close, "close", xPos, buttonWidth, IM_COL32(232, 17, 35, 255), [this] {
            windowService->close();
        });

        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, themeService->palette0);

        // Maximize/Restore Button
        xPos -= buttonWidth;
        rects.maximizeButton = {
            static_cast<int>(xPos), 0, static_cast<int>(buttonWidth), static_cast<int>(EditorPanel::HEADER_HEIGHT)
        };
        bool isMaximized = windowService->isMaximized();
        RenderWindowControl(isMaximized ? Icons::filter_none : Icons::crop_square, "maximize", xPos, buttonWidth,
                            IM_COL32(255, 255, 255, 30), [this] {
                                windowService->maximize();
                            });

        // Minimize Button
        xPos -= buttonWidth;
        rects.minimizeButton = {
            (int) xPos, 0, static_cast<int>(buttonWidth), static_cast<int>(EditorPanel::HEADER_HEIGHT)
        };
        RenderWindowControl(Icons::remove, "minimize", xPos, buttonWidth, IM_COL32(255, 255, 255, 30), [this] {
            windowService->minimize();
        });

        // Logo with Settings Icon that triggers Menu
        xPos -= EditorPanel::HEADER_HEIGHT;
        rects.historyButton = {
            static_cast<int>(xPos), 0, static_cast<int>(EditorPanel::HEADER_HEIGHT), static_cast<int>(EditorPanel::HEADER_HEIGHT)
        };
        ImGui::SetCursorPosX(xPos);
        ImGui::SetCursorPosY(0);
        if (UIUtil::ButtonSimple(Icons::history + "##history", EditorPanel::HEADER_HEIGHT, EditorPanel::HEADER_HEIGHT)) {
            ImGui::OpenPopup("HistoryMenu");
        }
        renderHistoryPopup();

        xPos -= EditorPanel::HEADER_HEIGHT;
        ImGui::SetCursorPosX(xPos);
        ImGui::SetCursorPosY(0);
        if (UIUtil::ButtonSimple(Icons::settings + "##settings", EditorPanel::HEADER_HEIGHT, EditorPanel::HEADER_HEIGHT)) {
            ImGui::OpenPopup("HeaderMenu");
        }
        renderHeaderMenuPopup();

        ImGui::PopStyleColor(1);
        ImGui::PopStyleVar();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::SetCursorPosX(5);
        ImGui::SetCursorPosY(0);

        renderLogo();

        rects.saveButton = {
            static_cast<int>(ImGui::GetCursorPosX()), 0, static_cast<int>(EditorPanel::HEADER_HEIGHT),
            static_cast<int>(EditorPanel::HEADER_HEIGHT)
        };
        if (UIUtil::ButtonSimple(Icons::save + "##save", EditorPanel::HEADER_HEIGHT, EditorPanel::HEADER_HEIGHT)) {
            directoryService->save();
        }
        ImGui::SameLine(0, 10);

        renderPlayStopButtons(rects);

        renderProjectName(rects);

        renderDockAdders(rects);

        float leftSideEnd = ImGui::GetCursorPosX();
        rects.dragArea = {
            static_cast<int>(leftSideEnd), 0, static_cast<int>(xPos - leftSideEnd),
            static_cast<int>(EditorPanel::HEADER_HEIGHT)
        };

        windowService->setWindowControlRects(rects);

        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);
    }

    void EditorHeaderPanel::renderLogo() {
        auto *logoTexture = textureService->stream("resources/logo.png");
        if (logoTexture) {
            ImGui::SetCursorPosY((EditorPanel::HEADER_HEIGHT - 20.0f) * 0.5f);
            imguiService->renderImage(logoTexture, 20, 20);
            ImGui::SameLine(0, 10);
        }
    }

    void EditorHeaderPanel::renderProjectName(WindowService::WindowControlRects &rects) {
        const float buttonWidth = 100.0f;
        const float containerHeight = 25.0f;
        
        rects.projectNameButton = {
            static_cast<int>(ImGui::GetCursorPosX()), 
            3,
            static_cast<int>(buttonWidth), 
            static_cast<int>(EditorPanel::HEADER_HEIGHT)
        };


        ImGui::SetCursorPosY(3);

        std::string name = editorRepository->projectName;
        if (UIUtil::RenderButtonSolid("projectNameBtn", name, ImVec2(buttonWidth, containerHeight), themeService->palette3, 4.0f)) {
            ImGui::OpenPopup("ProjectNamePopup");
        }
        UIUtil::RenderTooltip("Project: " + (name.empty() ? "[Untitled]" : name));

        if (ImGui::BeginPopup("ProjectNamePopup")) {
            char buffer[256];
            strncpy(buffer, editorRepository->projectName.c_str(), sizeof(buffer) - 1);
            buffer[sizeof(buffer) - 1] = '\0';
            if (ImGui::InputText("Name", buffer, sizeof(buffer))) {
                editorRepository->projectName = buffer;
            }
            ImGui::EndPopup();
        }
        ImGui::SameLine(0, 4);
    }

    void EditorHeaderPanel::renderPlayStopButtons(WindowService::WindowControlRects &rects) {
        const bool isPlaying = editorRepository->isPlaying;
        const float containerWidth = 53.0f;
        const float containerHeight = 25.0f;
        const float buttonSize = 18.0f;
        
        rects.playStopButton = {
            static_cast<int>(ImGui::GetCursorPosX()), 
            3,
            static_cast<int>(containerWidth), 
            static_cast<int>(containerHeight)
        };

        ImGui::SetCursorPosY(3);

        ImGui::PushStyleColor(ImGuiCol_ChildBg, themeService->palette3);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4.0f);
        if (ImGui::BeginChild("playStopContainer", ImVec2(containerWidth, containerHeight), false, ImGuiWindowFlags_NoScrollbar)) {
            float innerPadding = (containerHeight - buttonSize) * 0.5f;
            float spacing = (containerWidth - (buttonSize * 2)) / 3.0f;
            
            // Play Button
            ImGui::SetCursorPos(ImVec2(spacing, innerPadding));
            if (isPlaying) ImGui::BeginDisabled();
            ImVec4 playColor = isPlaying ? themeService->palette4 : ImVec4(0.298f, 0.686f, 0.314f, 1.0f);
            if (UIUtil::RenderButtonSolid("play", Icons::play_arrow, buttonSize, playColor, 4.0f)) {
                ApplicationEventContext::dispatch("Play");
            }
            if (isPlaying) ImGui::EndDisabled();
            UIUtil::RenderTooltip("Play");
            
            // Stop Button
            ImGui::SetCursorPos(ImVec2(spacing * 2 + buttonSize, innerPadding));
            if (!isPlaying) ImGui::BeginDisabled();
            ImVec4 stopColor = !isPlaying ? themeService->palette4 : ImVec4(0.957f, 0.263f, 0.212f, 1.0f);
            if (UIUtil::RenderButtonSolid("stop", Icons::stop, buttonSize, stopColor, 4.0f)) {
                ApplicationEventContext::dispatch("Stop");
            }
            if (!isPlaying) ImGui::EndDisabled();
            UIUtil::RenderTooltip("Stop");
            
            ImGui::EndChild();
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        ImGui::SameLine(0, 10);
    }

    void EditorHeaderPanel::renderHistoryPopup() {
        if (ImGui::BeginPopup("HistoryMenu")) {
            std::string snapshotsPath = directoryService->getEngineMetadataPath() + "/snapshots/" + directoryService->getProjectId();
            if (fs::exists(snapshotsPath)) {
                for (const auto &entry : fs::directory_iterator(snapshotsPath)) {
                    if (entry.is_directory()) {
                        if (ImGui::MenuItem(entry.path().filename().string().c_str())) {
                            snapshotService->restoreSnapshot(entry.path().filename().string());
                        }
                    }
                }
            } else {
                ImGui::TextDisabled("No snapshots available");
            }
            ImGui::EndPopup();
        }
    }

    void EditorHeaderPanel::renderHeaderMenuPopup() {
        if (ImGui::BeginPopup("HeaderMenu")) {
            if (ImGui::MenuItem("Save")) {
                directoryService->save();
            }
            if (ImGui::MenuItem("Open project")) {
                // TODO - Clear engine state and then load
                directoryService->updateRootPath(true);
            }
            if (ImGui::MenuItem("Reload shaders")) {
                // TODO - RECREATE ENGINE FRAMES ALL OF THEM
            }
            if (ImGui::MenuItem(editorRepository->isDarkMode ? "Light mode" : "Dark mode")) {
                editorRepository->isDarkMode = !editorRepository->isDarkMode;
            }
            if (ImGui::MenuItem("Clear caches")) {
                directoryService->clearCaches();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Close")) {
                windowService->close();
            }
            ImGui::EndPopup();
        }
        ImGui::SameLine(0, 5);
    }

    void EditorHeaderPanel::renderDockAdders(WindowService::WindowControlRects &rects) const {
        const float spacing = 2.0f;

        ImGui::SetCursorPosY(0);

        rects.dockAdders.push_back({
            static_cast<int>(ImGui::GetCursorPosX()), 0, static_cast<int>(EditorPanel::HEADER_HEIGHT),
            static_cast<int>(EditorPanel::HEADER_HEIGHT)
        });
        if (UIUtil::ButtonSimple(Icons::split_scene_left + "##addLeft", EditorPanel::HEADER_HEIGHT,
                                 EditorPanel::HEADER_HEIGHT)) {
            dockService->addLeftDock();
        }
        UIUtil::RenderTooltip("Add Left Dock");
        ImGui::SameLine(0, spacing);

        rects.dockAdders.push_back({
            static_cast<int>(ImGui::GetCursorPosX()), 0, static_cast<int>(EditorPanel::HEADER_HEIGHT),
            static_cast<int>(EditorPanel::HEADER_HEIGHT)
        });
        if (UIUtil::ButtonSimple(Icons::split_scene_down + "##addBottom", EditorPanel::HEADER_HEIGHT,
                                 EditorPanel::HEADER_HEIGHT)) {
            dockService->addBottomDock();
        }
        UIUtil::RenderTooltip("Add Bottom Dock");
        ImGui::SameLine(0, spacing);

        rects.dockAdders.push_back({
            static_cast<int>(ImGui::GetCursorPosX()), 0, static_cast<int>(EditorPanel::HEADER_HEIGHT),
            static_cast<int>(EditorPanel::HEADER_HEIGHT)
        });
        if (UIUtil::ButtonSimple(Icons::split_scene_right + "##addRight", EditorPanel::HEADER_HEIGHT,
                                 EditorPanel::HEADER_HEIGHT)) {
            dockService->addRightDock();
        }
        UIUtil::RenderTooltip("Add Right Dock");
    }

    void EditorHeaderPanel::RenderWindowControl(const std::string &icon, const char *id, float xPos, float width,
                                                ImU32 hoverColor, std::function<void()> action) {
        ImGui::SetCursorPosX(xPos);
        ImGui::SetCursorPosY(0);
        if (ImGui::Button((icon + "##" + id).c_str(), ImVec2(width, EditorPanel::HEADER_HEIGHT))) {
            action();
        }
        if (ImGui::IsItemHovered()) {
            ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), hoverColor);
        }
    }
}
