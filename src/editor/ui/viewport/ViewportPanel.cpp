#include "ViewportPanel.h"
#include "GizmoPanel.h"
#include "CameraGizmoPanel.h"
#include "ImGuizmo.h"
#include "ViewportHeaderPanel.h"
#include "EngineFramePanel.h"
#include "engine/dto/Camera.h"
#include "engine/frame-builder/EngineFrameBuilder.h"
#include <algorithm>
#include "../../repository/EditorRepository.h"
#include "engine/repository/WorldRepository.h"
#include "../../service/SelectionService.h"
#include "engine/service/CameraService.h"
#include "engine/repository/RuntimeRepository.h"
#include "engine/EngineContext.h"
#include "engine/dto/MetadataComponent.h"
#include "../../../ApplicationEventContext.h"

namespace Metal {
    void ViewportPanel::onInitialize() {
        headerPanel = initializePanel<ViewportHeaderPanel>();
        engineFramePanel = initializePanel<EngineFramePanel>();
        cameraGizmoPanel = initializePanel<CameraGizmoPanel>();
        gizmoPanel = initializePanel<GizmoPanel>(true, position, size);

        shortcuts = {
            ShortcutDTO("Change shading mode", ImGuiKey_Q, [this]() {
                editorRepository->shadingMode = ShadingModes::ValueOfIndex(
                    ShadingModes::IndexOfValue(editorRepository->shadingMode) + 1);
            }),
            ShortcutDTO("Translate", ImGuiKey_1, [this]() {
                editorRepository->gizmoType = ImGuizmo::OPERATION::TRANSLATE;
            }),
            ShortcutDTO("Scale", ImGuiKey_2, [this]() {
                editorRepository->gizmoType = ImGuizmo::OPERATION::SCALE;
            }),
            ShortcutDTO("Rotate", ImGuiKey_3, [this]() {
                editorRepository->gizmoType = ImGuizmo::OPERATION::ROTATE;
            }),
            ShortcutDTO("Delete", ImGuiKey_Delete, [this]() {
                std::vector<entt::entity> entities;
                for (auto &entry: editorRepository->selected) {
                    entities.push_back(entry.first);
                }
                worldRepository->deleteEntities(entities);
                selectionService->clearSelection();
            }),
            ShortcutDTO("Select All", ImGuiMod_Ctrl | ImGuiKey_A, [this]() {
                std::vector<entt::entity> entities;
                auto &storage = worldRepository->registry.storage<entt::entity>();
                for (auto it = storage.begin(); it != storage.end(); ++it) {
                    auto entity = *it;
                    if (worldRepository->registry.all_of<MetadataComponent>(entity)) {
                        entities.push_back(entity);
                    }
                }
                selectionService->addAllSelected(entities);
            }),
            ShortcutDTO("Move Forward", ImGuiKey_W, []() {
                ApplicationEventContext::dispatch("CameraMoveForward");
            }, true),
            ShortcutDTO("Move Backward", ImGuiKey_S, []() {
                ApplicationEventContext::dispatch("CameraMoveBackward");
            }, true),
            ShortcutDTO("Move Left", ImGuiKey_A, []() {
                ApplicationEventContext::dispatch("CameraMoveLeft");
            }, true),
            ShortcutDTO("Move Right", ImGuiKey_D, []() {
                ApplicationEventContext::dispatch("CameraMoveRight");
            }, true),
            ShortcutDTO("Move Up", ImGuiKey_Space, []() {
                ApplicationEventContext::dispatch("CameraMoveUp");
            }, true),
            ShortcutDTO("Move Down", ImGuiKey_LeftCtrl, []() {
                ApplicationEventContext::dispatch("CameraMoveDown");
            }, true)
        };
    }

    void ViewportPanel::onSync() {
        updateCamera();
        updateInputs();

        if (!editorRepository->isPlaying) {
            headerPanel->onSync();
        }
        engineFramePanel->onSync();

        if (!editorRepository->isPlaying) {
            gizmoPanel->onSync();
            cameraGizmoPanel->onSync();
        }
    }

    void ViewportPanel::updateCamera() {
        if (ImGui::IsWindowHovered() && !ImGuizmo::IsUsing() && ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            cameraService->handleInput(isFirstMovement);
            if (const auto &io = ImGui::GetIO(); io.MouseWheel != 0) {
                worldRepository->camera.movementSensitivity += io.MouseWheel * 100 * engineContext->deltaTime;
                worldRepository->camera.movementSensitivity =
                        std::max(.1f, worldRepository->camera.movementSensitivity);
            }
            isFirstMovement = false;
        } else {
            isFirstMovement = true;
        }
    }

    void ViewportPanel::updateInputs() const {
        runtimeRepository->viewportH = size->y;
        runtimeRepository->viewportW = size->x;

        const ImVec2 windowPos = ImGui::GetWindowPos();
        runtimeRepository->viewportX = windowPos.x;
        runtimeRepository->viewportY = windowPos.y;

        runtimeRepository->isFocused = ImGui::IsWindowHovered();
        runtimeRepository->mousePressed = ImGui::IsWindowFocused() && ImGui::IsWindowHovered() && ImGui::IsMouseDown(
                                ImGuiMouseButton_Left);

        const ImVec2 mousePos = ImGui::GetMousePos();
        runtimeRepository->mouseX = mousePos.x;
        runtimeRepository->mouseY = mousePos.y;

        runtimeRepository->normalizedMouseX = (runtimeRepository->mouseX + runtimeRepository->viewportX) / runtimeRepository->viewportW;
        runtimeRepository->normalizedMouseY = (runtimeRepository->viewportH - runtimeRepository->mouseY + runtimeRepository->viewportY) / runtimeRepository->viewportH;
    }
}
