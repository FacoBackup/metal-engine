#include "DockSpacePanel.h"
#include "../../common/UIUtil.h"

namespace Metal {
    const ImVec2 DockSpacePanel::DEFAULT{-1.f, -1.f};
    const ImVec2 DockSpacePanel::MAX_SIZE{MAX_FLOAT, MAX_FLOAT};
    const ImVec2 DockSpacePanel::PIVOT{0.5f, 0.5f};
    const float DockSpacePanel::FRAME_SIZE = 25;
    const ImVec2 DockSpacePanel::MIN_SIZE{300.f, 300.f};

    void DockSpacePanel::onInitialize() {
        Initializable::onInitialize();
    }

    void DockSpacePanel::initializeView() {

    }

    void DockSpacePanel::onSync() {
        Syncronizable::onSync();
    }

    AbstractDockPanel *DockSpacePanel::getView() {
        return nullptr;
    }

    void DockSpacePanel::renderHeader() {
        headerPadding.x = ImGui::GetStyle().FramePadding.x;

        if (ImGui::BeginMenuBar()) {
            std::string[] options = dock->description.getOptions();
            int &selected = dock->selectedOption;
            ImGui::SetNextItemWidth(ImGui::CalcTextSize(options[selected]).x + 30);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, headerPadding);
            if (ImGui::Combo(id.c_str(), selected, options)) {
                dock->description = dock->description.getSelectedOption(selected);
                initializeView();
            }
            ImGui::PopStyleVar();

            if (isNotCenter) {
                UIUtil::DynamicSpacing(55);
                if (ImGui::Button((isDownDirection ? Icons.horizontal_split : Icons.vertical_split) + "##splitView" + imguiId, ONLY_ICON_BUTTON_SIZE, ONLY_ICON_BUTTON_SIZE)) {
                    try {
                        DockDTO *dto = new DockDTO(dock->description.name );
                        dto->origin = dock;
                        dto->splitDir = isDownDirection ? ImGuiDir_Down : ImGuiDir_Right;
                        dto->sizeRatioForNodeAtDir = .5f;
                        dto->outAtOppositeDir = dock;
                        switch (dock->direction) {
                            case LEFT:
                                dockRepository.left.add(dockRepository.left.indexOf(dock) + 1, dto);
                                break;
                            case RIGHT:
                                dockRepository.right.add(dockRepository.right.indexOf(dock) + 1, dto);
                                break;
                            case BOTTOM:
                                dockRepository.bottom.add(dockRepository.bottom.indexOf(dock) + 1, dto);
                                break;
                            default:
                                break;
                        }
                        dockRepository.isInitialized = false;
                        messageRepository.pushMessage("Dock space created", MessageSeverity.SUCCESS);
                    } catch (Exception e) {
                        getLogger().error(e.getMessage(), e);
                        messageRepository.pushMessage("Error while creating dock space", MessageSeverity.ERROR);
                    }
                }

                if (ImGui::Button(Icons.close + "##removeView" + imguiId, ONLY_ICON_BUTTON_SIZE, ONLY_ICON_BUTTON_SIZE)) {
                    dockService.prepareForRemoval(dock, this);
                }
            }
            ImGui::EndMenuBar();
        }
    }

    void DockSpacePanel::beforeWindow() {
        if (mainWindow != nullptr && mainWindow != this) {
            ImVec2 pos = mainWindow->getPosition();
            ImVec2 sze = mainWindow->getSize();
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