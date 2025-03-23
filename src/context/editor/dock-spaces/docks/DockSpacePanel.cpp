#include "DockSpacePanel.h"

#include "AbstractDockPanel.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../repository/dock/DockDTO.h"
#include "../../../../util/UIUtil.h"

namespace Metal {
    const ImVec2 DockSpacePanel::DEFAULT{-1.f, -1.f};
    const ImVec2 DockSpacePanel::MAX_SIZE{MAX_FLOAT, MAX_FLOAT};
    const ImVec2 DockSpacePanel::PIVOT{0.5f, 0.5f};
    const float DockSpacePanel::FRAME_SIZE = 25;
    const ImVec2 DockSpacePanel::MIN_SIZE{300.f, 300.f};

    void DockSpacePanel::onInitialize() {
        initializeView();
    }

    void DockSpacePanel::initializeView() {
        removeAllChildren();
    }

    void DockSpacePanel::onSync() {
        ImGui::SetNextWindowSizeConstraints(MIN_SIZE, MAX_SIZE);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2, 2));
        stylePushCount++;
        if (!sizeInitialized && dock->sizeX > 0 && dock->sizeY > 0) {
            UIUtil::AUX_VEC2.x = dock->sizeX;
            UIUtil::AUX_VEC2.y = dock->sizeY;
            ImGui::SetNextWindowSize(UIUtil::AUX_VEC2);
            sizeInitialized = true;
        }
        beforeWindow();
        if (ImGui::Begin(dock->internalId.c_str(), &UIUtil::OPEN,
                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove)) {
            sizeInternal = ImGui::GetWindowSize();
            size.x = sizeInternal.x;
            size.y = sizeInternal.y;

            dock->sizeX = size.x;
            dock->sizeY = size.y;

            position = ImGui::GetWindowPos();
            renderTabs();
        }
        ImGui::End();

        ImGui::PopStyleVar(stylePushCount);
        stylePushCount = 0;
    }

    void DockSpacePanel::renderView(DockViewDTO &viewDTO) {
        DockSpace *dockData = DockSpace::GetOption(viewDTO.dockSpaceIndex);
        if (viewDTO.view == nullptr) {
            viewDTO.view = dockData->getPanel();
            viewDTO.view->size = &size;
            viewDTO.view->dock = dock;
            viewDTO.view->position = &position;
            appendChild(viewDTO.view);
        }
        viewDTO.view->isWindowFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
        viewDTO.view->onSync();
    }

    void DockSpacePanel::renderTabs() {
        if (dock->direction == CENTER) {
            renderView(dock->openTabs[0]);
            return;
        }
        if (ImGui::BeginTabBar((id + "MyTabBar").c_str())) {
            int tabToClose = -1; // Track which tab should be closed

            for (size_t i = 0; i < dock->openTabs.size(); ++i) {
                auto &viewDTO = dock->openTabs[i];
                DockSpace *dockData = DockSpace::GetOption(viewDTO.dockSpaceIndex);

                bool open = true; // Used for close behavior
                if (ImGui::BeginTabItem((dockData->icon + " " + dockData->name).c_str(), &open,
                                        ImGuiTabItemFlags_None)) {
                    renderView(viewDTO);
                    ImGui::EndTabItem();
                }

                if (!open) {
                    tabToClose = static_cast<int>(i);
                }

                if (ImGui::BeginPopupContextItem((id + "TabContext" + std::to_string(i)).c_str(),
                                                 ImGuiPopupFlags_MouseButtonRight)) {
                    if (ImGui::MenuItem("Close Tab")) {
                        tabToClose = static_cast<int>(i);
                    }
                    ImGui::EndPopup();
                }
            }

            if (tabToClose != -1) {
                removeChild(dock->openTabs[tabToClose].view);
                dock->openTabs.erase(dock->openTabs.begin() + tabToClose);
            }

            if (ImGui::BeginTabItem("+")) {
                ImGui::Text("Select a New Tab:");
                ImGui::Separator();

                for (int i = 0; i < DockSpace::GetOptionCount(); ++i) {
                    DockSpace *dockOption = DockSpace::GetOption(i);
                    std::string tabLabel = dockOption->icon + " " + dockOption->name;

                    if (ImGui::Selectable(tabLabel.c_str())) {
                        dock->addNewTab(i);
                        ImGui::CloseCurrentPopup();
                    }
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }

    void DockSpacePanel::beforeWindow() const {
        if (mainWindow != nullptr && mainWindow != this) {
            const ImVec2 &pos = mainWindow->getPosition();
            const ImVec2 &sze = mainWindow->getSize();
            UIUtil::AUX_VEC2.x = pos.x + sze.x * 0.5f;
            UIUtil::AUX_VEC2.y = pos.y + sze.y * 0.5f;
            ImGui::SetNextWindowPos(UIUtil::AUX_VEC2, ImGuiCond_FirstUseEver, PIVOT);
        }
    }

    ImVec2 &DockSpacePanel::getPosition() {
        return position;
    }

    ImVec2 &DockSpacePanel::getSize() {
        return sizeInternal;
    }
} // Metal
