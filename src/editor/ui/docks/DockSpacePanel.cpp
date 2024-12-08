#include "DockSpacePanel.h"

#include "AbstractDockPanel.h"
#include "../../../common/runtime/ApplicationContext.h"
#include "../../../common/interface/Icons.h"
#include "../../repository/dock/DockDTO.h"
#include "../../common/UIUtil.h"

namespace Metal {
    const ImVec2 DockSpacePanel::DEFAULT{-1.f, -1.f};
    const ImVec2 DockSpacePanel::MAX_SIZE{MAX_FLOAT, MAX_FLOAT};
    const ImVec2 DockSpacePanel::PIVOT{0.5f, 0.5f};
    const float DockSpacePanel::FRAME_SIZE = 25;
    const ImVec2 DockSpacePanel::MIN_SIZE{300.f, 300.f};

    void DockSpacePanel::onInitialize() {
        initializeView();
        isNotCenter = dock->direction != CENTER;
    }

    void DockSpacePanel::initializeView() {
        removeAllChildren();
        view = dock->description->getPanel();
        view->size = &size;
        view->dock = dock;
        view->position = &position;
        appendChild(view);
    }

    void DockSpacePanel::onSync() {
        ImGui::SetNextWindowSizeConstraints(MIN_SIZE, MAX_SIZE);
        if (padding.x != DEFAULT.x || padding.y != DEFAULT.y) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding);
            stylePushCount++;
        }
        if (!sizeInitialized && dock->sizeX > 0 && dock->sizeY > 0) {
            UIUtil::AUX_VEC2.x = dock->sizeX;
            UIUtil::AUX_VEC2.y = dock->sizeY;
            ImGui::SetNextWindowSize(UIUtil::AUX_VEC2);
            sizeInitialized = true;
        }
        beforeWindow();
        if (ImGui::Begin(dock->internalId, &UIUtil::OPEN, FLAGS)) {
            view->isWindowFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
            sizeInternal = ImGui::GetWindowSize();
            size.x = sizeInternal.x;
            size.y = sizeInternal.y;

            dock->sizeX = size.x;
            dock->sizeY = size.y;

            position = ImGui::GetWindowPos();
            renderHeader();
            view->onSync();
        }
        ImGui::End();

        ImGui::PopStyleVar(stylePushCount);
        stylePushCount = 0;
    }

    AbstractDockPanel *DockSpacePanel::getView() const {
        return view;
    }

    void DockSpacePanel::renderHeader() {
        DockRepository &dockRepository = context->getEditorContext().dockRepository;
        DockService &dockService = context->getEditorContext().dockService;

        headerPadding.x = ImGui::GetStyle().FramePadding.x;

        if (ImGui::BeginMenuBar()) {
            int &selected = dock->selectedOption;
            ImGui::SetNextItemWidth(ImGui::CalcTextSize(DockSpace::GetOption(selected)->name.c_str()).x + 30);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, headerPadding);
            if (ImGui::Combo(id.c_str(), &selected, DockSpace::OPTIONS)) {
                dock->description = DockSpace::GetOption(selected);
                initializeView();
            }
            ImGui::PopStyleVar();

            if (isNotCenter) {
                UIUtil::DynamicSpacing(55);
                if (UIUtil::ButtonSimple(
                    (isDownDirection ? Icons::horizontal_split : Icons::vertical_split) + id + "splitView",
                    UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
                    auto *dto = new DockDTO(dock->description);
                    dto->origin = dock;
                    dto->splitDir = isDownDirection ? ImGuiDir_Down : ImGuiDir_Right;
                    dto->sizeRatioForNodeAtDir = .5f;
                    dto->outAtOppositeDir = dock;
                    switch (dock->direction) {
                        case LEFT:
                            dockRepository.left.insert(
                                dockRepository.left.begin() + Util::indexOf(dockRepository.left, dock) + 1, dto);

                            break;
                        case RIGHT:
                            dockRepository.right.insert(
                                dockRepository.right.begin() + Util::indexOf(dockRepository.right, dock) + 1, dto);
                            break;
                        case BOTTOM:
                            dockRepository.bottom.insert(
                                dockRepository.bottom.begin() + Util::indexOf(dockRepository.bottom, dock) + 1,
                                dto);

                            break;
                        default:
                            break;
                    }
                    dockRepository.isInitialized = false;
                }

                if (UIUtil::ButtonSimple(Icons::close + id + "removeView", UIUtil::ONLY_ICON_BUTTON_SIZE,
                                         UIUtil::ONLY_ICON_BUTTON_SIZE)) {
                    dockService.prepareForRemoval(dock, this);
                }
            }
            ImGui::EndMenuBar();
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
