#include "ViewportPanel.h"

#include "CameraPositionPanel.h"
#include "GizmoPanel.h"
#include "ImGuizmo.h"
#include "ViewportHeaderPanel.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../service/descriptor/DescriptorInstance.h"
#include "../../../../service/framebuffer/FrameBufferInstance.h"
#include "../../../../service/camera/Camera.h"

#include <algorithm>

namespace Metal {
    void ViewportPanel::onInitialize() {
        appendChild(headerPanel = new ViewportHeaderPanel());
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

        const float tabHeight = ImGui::GetFrameHeightWithSpacing();
        const ImVec2 viewportSize{size->x, size->y - tabHeight - ViewportHeaderPanel::HEIGHT};

        auto *framebuffer = CTX.coreFrameBuffers.postProcessingFBO;
        CTX.descriptorSetService.setImageDescriptor(framebuffer, 0);
        ImGui::Image(reinterpret_cast<ImTextureID>(framebuffer->attachments[0]->imageDescriptor->vkDescriptorSet),
                     viewportSize);

        const ImVec2 imageMin = ImGui::GetItemRectMin();
        const ImVec2 imageMax = ImGui::GetItemRectMax();
        handleViewportPicking(imageMin, imageMax);

        gizmoPanel->onSync();
        cameraPanel->onSync();
    }

    void ViewportPanel::handleViewportPicking(const ImVec2 &imageMin, const ImVec2 &imageMax) const {
        if (!ImGui::IsItemHovered() || !ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            return;
        }
        if (ImGuizmo::IsUsing() || ImGuizmo::IsOver()) {
            return;
        }

        const ImVec2 mousePos = ImGui::GetMousePos();
        const float imageW = imageMax.x - imageMin.x;
        const float imageH = imageMax.y - imageMin.y;
        if (imageW <= 1.0f || imageH <= 1.0f) {
            return;
        }

        const float u = (mousePos.x - imageMin.x) / imageW;
        const float v = (mousePos.y - imageMin.y) / imageH;
        if (u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f) {
            return;
        }

        auto *gBuffer = CTX.coreFrameBuffers.gBufferFBO;
        if (!gBuffer) {
            return;
        }
        const auto width = gBuffer->bufferWidth;
        const auto height = gBuffer->bufferHeight;
        const uint32_t pixelX = std::min(static_cast<uint32_t>(u * static_cast<float>(width)), width - 1);
        const uint32_t pixelY = std::min(static_cast<uint32_t>(v * static_cast<float>(height)), height - 1);

        const auto picked = CTX.pickingService.pickEntityFromGBuffer(pixelX, pixelY);
        CTX.selectionService.clearSelection();
        CTX.selectionService.addSelected(picked.value_or(EMPTY_ENTITY));
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
