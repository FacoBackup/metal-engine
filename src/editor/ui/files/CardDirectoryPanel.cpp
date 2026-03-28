#include "CardDirectoryPanel.h"
#include "editor/dto/FSEntry.h"
#include "engine/service/TextureService.h"
#include "../../../core/ImGuiService.h"
#include "common/Icons.h"
#include "editor/ui/UIUtil.h"
#include "common/FileExtensions.h"
#include <algorithm>

#define CARD_WIDTH 120.0f
#define CARD_HEIGHT 120.0f
#define CARD_SIZE ImVec2(CARD_WIDTH, CARD_HEIGHT)
#define PREVIEW_SIZE 70.0f
#define FOOTER_HEIGHT 30.0f
#define ICON_Y_OFFSET 20.0f
#define ICON_SPACING 10.0f
#define NAME_HORIZONTAL_MARGIN 10.0f

namespace Metal {
    void CardDirectoryPanel::renderCard(std::shared_ptr<FSEntry> entry) {
        ImGui::BeginGroup();

        bool isSelected = filesContext.selected.contains(entry->absolutePath);
        ImGui::PushID(entry->absolutePath.c_str());

        if (isSelected) {
            ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]);
        }

        const ImGuiChildFlags childFlags = ImGuiChildFlags_Border | ImGuiChildFlags_FrameStyle |
                                           ImGuiChildFlags_FrameStyle;
        const ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

        if (ImGui::BeginChild(entry->absolutePath.c_str(), CARD_SIZE, childFlags, windowFlags)) {
            if (isSelected) {
                ImDrawList *drawList = ImGui::GetWindowDrawList();
                ImVec2 pMin = ImGui::GetWindowPos();
                ImVec2 pMax = ImVec2(pMin.x + CARD_WIDTH, pMin.y + CARD_HEIGHT);
                drawList->AddRectFilled(pMin, pMax, ImGui::GetColorU32(ImGuiCol_HeaderActive));
            }
            // Preview
            ImGui::SetCursorPosX((CARD_WIDTH - PREVIEW_SIZE) * 0.5f);

            bool previewRendered = false;
            static const std::vector<std::string> textureExtensions = {
                Metal::FileExtensions::png->extension, Metal::FileExtensions::jpg->extension,
                Metal::FileExtensions::jpeg->extension,
                Metal::FileExtensions::tga->extension
            };
            if (!entry->isDirectory && std::ranges::find(textureExtensions, entry->extension) != textureExtensions.
                end()) {
                auto *texture = textureService->stream(entry->absolutePath);
                if (texture != nullptr) {
                    imguiService->renderImage(texture, PREVIEW_SIZE, PREVIEW_SIZE);
                    previewRendered = true;
                }
            }

            if (!previewRendered) {
                // Render icon
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ICON_Y_OFFSET);
                const std::string icon = getEntryIcon(entry);

                ImGui::PushFont(imguiService->getLargeIconsFont());
                float textWidth = ImGui::CalcTextSize(icon.c_str()).x;
                ImGui::SetCursorPosX((CARD_WIDTH - textWidth) * 0.5f);
                ImGui::Text("%s", icon.c_str());
                ImGui::PopFont();
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ICON_SPACING);
            }

            // Name
            ImGui::SetCursorPosY(CARD_HEIGHT - FOOTER_HEIGHT);
            ImGui::Separator();
            std::string displayName = entry->name;
            if (ImGui::CalcTextSize(displayName.c_str()).x > CARD_WIDTH - NAME_HORIZONTAL_MARGIN) {
                displayName = displayName.substr(0, 10) + "...";
            }
            float nameWidth = ImGui::CalcTextSize(displayName.c_str()).x;
            ImGui::SetCursorPosX((CARD_WIDTH - nameWidth) * 0.5f);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (FOOTER_HEIGHT - ImGui::GetTextLineHeight()) * 0.5f);
            ImGui::Text("%s", displayName.c_str());
        }
        ImGui::EndChild();

        if (isSelected) {
            ImGui::PopStyleColor(2);
        }

        if (ImGui::IsItemClicked()) {
            if (ImGui::IsKeyDown(ImGuiMod_Ctrl)) {
                if (isSelected) filesContext.selected.erase(entry->absolutePath);
                else filesContext.selected.insert({entry->absolutePath, entry});
            } else {
                filesContext.selected.clear();
                filesContext.selected.insert({entry->absolutePath, entry});
            }
        }

        if (ImGui::IsItemHovered()) {
            renderInfoTooltip(entry);
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                openResource(entry);
            }
        }

        ImGui::PopID();
        ImGui::EndGroup();
    }

    void CardDirectoryPanel::onSync() {
        if (ImGui::BeginChild((id + "card_view").c_str(), ImVec2(0, 0))) {
            if (ImGui::IsWindowFocused() && !ImGui::IsAnyItemHovered()) {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    filesContext.selected.clear();
                }
            }

            float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
            ImGuiStyle &style = ImGui::GetStyle();

            auto currentDir = filesContext.currentDirectory;
            for (const auto &child: currentDir->children) {
                if (matchesSearch(child)) {
                    renderCard(child);
                    float last_button_x2 = ImGui::GetItemRectMax().x;
                    float next_button_x2 = last_button_x2 + style.ItemSpacing.x + CARD_WIDTH;
                    // Expected position if next button was on same line
                    if (next_button_x2 < window_visible_x2)
                        ImGui::SameLine();
                }
            }
        }
        ImGui::EndChild();
    }
}
