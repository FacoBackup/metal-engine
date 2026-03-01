#include "ViewportPanel.h"

#include "CameraPositionPanel.h"
#include "GizmoPanel.h"
#include "ImGuizmo.h"
#include "ViewportHeaderPanel.h"
#include "EngineFramePanel.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../service/descriptor/DescriptorInstance.h"
#include "../../../../service/framebuffer/FrameBufferInstance.h"
#include "../../../../service/camera/Camera.h"
#include "../../../../context/engine/frame-builder/EngineFrameBuilder.h"
#include "../../../../service/framebuffer/FrameBufferService.h"
#include "../../../../enum/engine-definitions.h"
#include "../../../../dto/buffers/GlobalDataUBO.h"
#include "../../../../dto/buffers/TileInfoUBO.h"
#include "../../../../dto/buffers/LightData.h"
#include "../../../../dto/buffers/VolumeData.h"
#include "../../../../dto/buffers/MaterialData.h"

#include <algorithm>

namespace Metal {
    void ViewportPanel::onInitialize() {
        appendChild(headerPanel = new ViewportHeaderPanel());
        appendChild(engineFramePanel = new EngineFramePanel());
        appendChild(gizmoPanel = new GizmoPanel(position, size));
        appendChild(cameraPanel = new CameraPositionPanel());

        shortcuts = {
            ShortcutDTO("Change shading mode", ImGuiKey_Q, []() {
                CTX.editorRepository.shadingMode = ShadingModes::ValueOfIndex(
                    ShadingModes::IndexOfValue(CTX.editorRepository.shadingMode) + 1);
            }),
            ShortcutDTO("Translate", ImGuiKey_1, []() {
                CTX.editorRepository.gizmoType = ImGuizmo::OPERATION::TRANSLATE;
            }),
            ShortcutDTO("Scale", ImGuiKey_2, []() {
                CTX.editorRepository.gizmoType = ImGuizmo::OPERATION::SCALE;
            }),
            ShortcutDTO("Rotate", ImGuiKey_3, []() {
                CTX.editorRepository.gizmoType = ImGuizmo::OPERATION::ROTATE;
            }),
            ShortcutDTO("Delete", ImGuiKey_Delete, [this]() {
                std::vector<EntityID> entities;
                for (auto &entry: CTX.editorRepository.selected) {
                    entities.push_back(entry.first);
                }
                CTX.worldRepository.deleteEntities(entities);
                CTX.selectionService.clearSelection();
            }),
            ShortcutDTO("Select All", ImGuiMod_Ctrl | ImGuiKey_A, [this]() {
                std::vector<EntityID> entities;
                auto &storage = CTX.worldRepository.registry.storage<entt::entity>();
                for (auto it = storage.begin(); it != storage.end(); ++it) {
                    auto entity = *it;
                    if (static_cast<EntityID>(entity) != WorldRepository::ROOT_ID && CTX.worldRepository.registry.all_of
                        <EntityComponent>(entity)) {
                        entities.push_back(static_cast<EntityID>(entity));
                    }
                }
                CTX.selectionService.addAllSelected(entities);
            }),
            ShortcutDTO("Save", ImGuiMod_Ctrl | ImGuiKey_S, [] {
                CTX.save();
            })
        };
    }

    void ViewportPanel::onSync() {
        if (!CTX.engineRepository.isBaking) {
            updateCamera();
            updateInputs();
        }

        headerPanel->onSync();
        engineFramePanel->onSync();

        gizmoPanel->onSync();
        cameraPanel->onSync();
    }

    void ViewportPanel::updateCamera() {
        auto &worldRepository = CTX.worldRepository;
        const auto &cameraService = CTX.cameraService;

        if (ImGui::IsWindowHovered() && !ImGuizmo::IsUsing() && ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            cameraService.handleInput(isFirstMovement);
            if (const auto &io = ImGui::GetIO(); io.MouseWheel != 0) {
                worldRepository.camera.movementSensitivity += io.MouseWheel * 100 * CTX.
                        engineContext.deltaTime;
                worldRepository.camera.movementSensitivity =
                        std::max(.1f, worldRepository.camera.movementSensitivity);
            }
            isFirstMovement = false;
        } else {
            isFirstMovement = true;
        }
    }

    void ViewportPanel::updateInputs() const {
        auto &repo = CTX.runtimeRepository;
        const ImVec2 windowSize = ImGui::GetWindowSize();
        size->x = windowSize.x;
        size->y = windowSize.y;

        repo.viewportH = size->y;
        repo.viewportW = size->x;

        const ImVec2 windowPos = ImGui::GetWindowPos();
        repo.viewportX = windowPos.x;
        repo.viewportY = windowPos.y;

        repo.isFocused = ImGui::IsWindowHovered();
        repo.forwardPressed = ImGui::IsKeyDown(ImGuiKey_W);
        repo.backwardPressed = ImGui::IsKeyDown(ImGuiKey_S);
        repo.leftPressed = ImGui::IsKeyDown(ImGuiKey_A);
        repo.rightPressed = ImGui::IsKeyDown(ImGuiKey_D);
        repo.upPressed = ImGui::IsKeyDown(ImGuiKey_Space);
        repo.downPressed = ImGui::IsKeyDown(ImGuiKey_LeftCtrl);
        repo.mousePressed = ImGui::IsWindowFocused() && ImGui::IsWindowHovered() && ImGui::IsMouseDown(
                                ImGuiMouseButton_Left);

        const ImVec2 mousePos = ImGui::GetMousePos();
        repo.mouseX = mousePos.x;
        repo.mouseY = mousePos.y;

        repo.normalizedMouseX = (repo.mouseX + repo.viewportX) / repo.viewportW;
        repo.normalizedMouseY = (repo.viewportH - repo.mouseY + repo.viewportY) / repo.viewportH;
    }
}
