#include "EngineFramePanel.h"
#include "ApplicationContext.h"
#include "engine/frame-builder/EngineFrameBuilder.h"
#include "engine/frame-builder/EngineFrame.h"
#include "engine/dto/DescriptorInstance.h"
#include "engine/resource/RenderTargetInstance.h"
#include "../../service/PickingService.h"
#include "../../enum/engine-definitions.h"
#include "../../enum/EngineResourceIDs.h"
#include "engine/dto/GlobalDataUBO.h"
#include "engine/dto/LightData.h"
#include "engine/dto/MaterialData.h"
#include "engine/dto/PrimitiveData.h"
#include "ViewportHeaderPanel.h"
#include "ImGuizmo.h"
#include "../../repository/EditorRepository.h"
#include <algorithm>
#include "engine/passes/impl/DepthOfFieldPass.h"
#include "engine/passes/impl/HWRayTracingPass.h"
#include "engine/passes/impl/MotionBlurPass.h"
#include "engine/passes/impl/PostProcessingPass.h"
#include "engine/passes/impl/StaticGBufferPass.h"
#include "../../passes/GridPass.h"
#include "../../passes/SelectionIDPass.h"
#include "../../passes/SelectionOutlinePass.h"
#include "../../passes/CameraVisualizationPass.h"
#include "../../../core/VulkanContext.h"
#include "engine/repository/EngineRepository.h"
#include "engine/EngineContext.h"
#include "engine/service/DescriptorSetService.h"
#include "../../service/SelectionService.h"
#include "engine/resource/TextureInstance.h"

#include "editor/repository/EditorCameraRepository.h"
#include "editor/service/EditorCameraService.h"
#include "engine/repository/WorldRepository.h"
#include "engine/dto/CameraComponent.h"
#include "engine/dto/TransformComponent.h"

#include "editor/ui/UIUtil.h"
#include <glm/gtc/type_ptr.hpp>

namespace Metal {
    void EngineFramePanel::onInitialize() {
        engineContext->setCameraUpdateCallback([this]() {
            if (!editorRepository->isPlaying) {
                worldRepository->camera.fov = editorCameraRepository->fov;
                worldRepository->camera.zNear = editorCameraRepository->zNear;
                worldRepository->camera.zFar = editorCameraRepository->zFar;
                worldRepository->camera.rotationSensitivity = editorCameraRepository->rotationSensitivity;
                worldRepository->camera.movementSensitivity = editorCameraRepository->movementSensitivity;
                worldRepository->camera.zoomSensitivity = editorCameraRepository->zoomSensitivity;
                worldRepository->camera.motionBlurEnabled = editorCameraRepository->motionBlurEnabled;
                worldRepository->camera.motionBlurVelocityScale = editorCameraRepository->motionBlurVelocityScale;
                worldRepository->camera.motionBlurMaxSamples = editorCameraRepository->motionBlurMaxSamples;
                worldRepository->camera.cameraMotionBlur = editorCameraRepository->cameraMotionBlur;
                worldRepository->camera.dofEnabled = editorCameraRepository->dofEnabled;
                worldRepository->camera.dofFocusDistance = editorCameraRepository->dofFocusDistance;
                worldRepository->camera.dofAperture = editorCameraRepository->dofAperture;
                worldRepository->camera.dofFocalLength = editorCameraRepository->dofFocalLength;
                worldRepository->camera.bloomEnabled = editorCameraRepository->bloomEnabled;
                worldRepository->camera.filmGrain = editorCameraRepository->filmGrain;
                worldRepository->camera.vignetteEnabled = editorCameraRepository->vignetteEnabled;
                worldRepository->camera.chromaticAberrationEnabled = editorCameraRepository->chromaticAberrationEnabled;
                worldRepository->camera.distortionEnabled = editorCameraRepository->distortionEnabled;
                worldRepository->camera.filmGrainStrength = editorCameraRepository->filmGrainStrength;
                worldRepository->camera.vignetteStrength = editorCameraRepository->vignetteStrength;
                worldRepository->camera.bloomThreshold = editorCameraRepository->bloomThreshold;
                worldRepository->camera.bloomQuality = editorCameraRepository->bloomQuality;
                worldRepository->camera.bloomOffset = editorCameraRepository->bloomOffset;
                worldRepository->camera.chromaticAberrationIntensity = editorCameraRepository->
                        chromaticAberrationIntensity;
                worldRepository->camera.distortionIntensity = editorCameraRepository->distortionIntensity;
                worldRepository->camera.position = editorCameraRepository->position;
                worldRepository->camera.pitch = editorCameraRepository->pitch;
                worldRepository->camera.yaw = editorCameraRepository->yaw;
            } else {
                auto view = worldRepository->registry.view<CameraComponent, TransformComponent>();
                for (auto entity: view) {
                    auto &cameraComp = view.get<CameraComponent>(entity);
                    if (cameraComp.mainCamera) {
                        worldRepository->updateCameraData(entity);
                        break;
                    }
                }
            }
        });

        const auto gBufferW = vulkanContext->getWindowWidth() / engineRepository->
                              shadingResInvScale;
        const auto gBufferH = vulkanContext->getWindowHeight() / engineRepository
                              ->shadingResInvScale;

        engineContext->createFrame(id)
                .addBuffer(RID_GLOBAL_DATA, sizeof(GlobalDataUBO),
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, UNIFORM_BUFFER)
                .addBuffer(RID_LIGHT_BUFFER, MAX_LIGHTS * sizeof(LightData),
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, STORAGE_BUFFER)
                .addBuffer(RID_MATERIAL_DATA_BUFFER, MAX_MESH_INSTANCES * sizeof(MaterialData),
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, STORAGE_BUFFER)
                .addBuffer(RID_PRIMITIVE_DATA_BUFFER, MAX_MESH_INSTANCES * sizeof(PrimitiveData),
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, STORAGE_BUFFER)
                .addBuffer(RID_RESTIR_RESERVOIR_0, gBufferW * gBufferH * 40, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                           STORAGE_BUFFER)
                .addBuffer(RID_RESTIR_RESERVOIR_1, gBufferW * gBufferH * 40, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                           STORAGE_BUFFER)
                .addTexture(RID_ACCUMULATED_FRAME, gBufferW, gBufferH, VK_FORMAT_R32G32B32A32_SFLOAT)

                .addRenderTarget(RID_GBUFFER_RT, gBufferW, gBufferH, glm::vec4(0, 0, 0, 0))
                .addColor(VK_FORMAT_R16G16B16A16_SFLOAT,
                          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT) // 0: Albedo
                .addColor(VK_FORMAT_R16G16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT) // 1: Roughness/Metallic
                .addColor(VK_FORMAT_R32G32_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT) // 2: RenderIndex/Depth
                .addColor(VK_FORMAT_R16G16B16A16_SFLOAT,
                          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT) // 3: Normal
                .addColor(VK_FORMAT_R16G16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT) // 4: Motion Vectors
                .addDepth()

                .addRenderTarget(RID_SELECTION_RT, gBufferW, gBufferH, glm::vec4(0, 0, 0, 0))
                .addColor(VK_FORMAT_R16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)

                .addRenderTarget(RID_POST_PROCESSING_RT, gBufferW, gBufferH, glm::vec4(0, 0, 0, 0))
                .addColor(VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)

                .addRenderTarget(RID_DOF_RT, gBufferW, gBufferH, glm::vec4(0, 0, 0, 0))
                .addColor(VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)

                .addRenderTarget(RID_MOTION_BLUR_RT, gBufferW, gBufferH, glm::vec4(0, 0, 0, 0))
                .addColor(VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)

                .addCommandBuffer(RID_GBUFFER_CB, RID_GBUFFER_RT) // 0
                .addPass(std::make_unique<StaticGBufferPass>(), RID_GBUFFER_CB)

                .addComputeCommandBuffer(RID_RT_COMPUTE_CB, true) // 1
                .addPass(std::make_unique<HWRayTracingPass>(), RID_RT_COMPUTE_CB)

                .addCommandBuffer(RID_DOF_CB, RID_DOF_RT) // 2
                .addPass(std::make_unique<DepthOfFieldPass>(), RID_DOF_CB)

                .addCommandBuffer(RID_MOTION_BLUR_CB, RID_MOTION_BLUR_RT) // 3
                .addPass(std::make_unique<MotionBlurPass>(), RID_MOTION_BLUR_CB)

                .addCommandBuffer(RID_SELECTION_CB, RID_SELECTION_RT) // 4
                .addPass(std::make_unique<SelectionIDPass>(), RID_SELECTION_CB)

                .addCommandBuffer(RID_POST_PROCESSING_CB, RID_POST_PROCESSING_RT) // 5
                .addPass(std::make_unique<PostProcessingPass>(), RID_POST_PROCESSING_CB)
                .addPass(std::make_unique<GridPass>(), RID_POST_PROCESSING_CB)
                .addPass(std::make_unique<CameraVisualizationPass>(), RID_POST_PROCESSING_CB)
                .addPass(std::make_unique<SelectionOutlinePass>(), RID_POST_PROCESSING_CB)
                .build();
    }

    void EngineFramePanel::onSync() {
        engineContext->setCurrentFrame(id);

        const ImVec2 viewportSize = ImGui::GetContentRegionAvail();

        auto *framebuffer = engineContext->currentFrame->getResourceAs<RenderTargetInstance>(RID_POST_PROCESSING_RT);
        if (framebuffer) {
            descriptorSetService->setImageDescriptor(framebuffer, 0);

            const ImVec2 pos = ImGui::GetCursorScreenPos();
            const auto textureId = reinterpret_cast<ImTextureID>(framebuffer->attachments[0]->imageDescriptor->
                vkDescriptorSet);

            ImGui::GetWindowDrawList()->AddImageRounded(textureId, pos,
                                                        ImVec2(pos.x + viewportSize.x, pos.y + viewportSize.y),
                                                        ImVec2(0, 0), ImVec2(1, 1), IM_COL32_WHITE, 8.0f);

            ImGui::Dummy(viewportSize);

            const ImVec2 imageMin = pos;
            const ImVec2 imageMax = ImVec2(pos.x + viewportSize.x, pos.y + viewportSize.y);

            if (!editorRepository->isPlaying) {
                handleViewportPicking(imageMin, imageMax);
            }
        }
    }

    void EngineFramePanel::handleViewportPicking(const ImVec2 &imageMin, const ImVec2 &imageMax) const {
        if (!ImGui::IsItemHovered() || !ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            return;
        }
        if (editorRepository->isGizmoOver || editorRepository->isGizmoUsing) {
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

        RenderTargetInstance *gBuffer = engineContext->currentFrame->getResourceAs<RenderTargetInstance>(
            RID_GBUFFER_RT);

        const auto width = gBuffer->bufferWidth;
        const auto height = gBuffer->bufferHeight;
        const uint32_t pixelX = std::min(static_cast<uint32_t>(u * static_cast<float>(width)), width - 1);
        const uint32_t pixelY = std::min(static_cast<uint32_t>(v * static_cast<float>(height)), height - 1);

        const auto picked = pickingService->pickEntityFromGBuffer(
            gBuffer->attachments[RID_GBUFFER_RENDER_INDEX_DEPTH], pixelX, pixelY);
        selectionService->clearSelection();
        selectionService->addSelected(picked.value_or(EMPTY_ENTITY));
    }
} // Metal
