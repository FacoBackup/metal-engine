#include "EngineFramePanel.h"
#include "../../../ApplicationContext.h"
#include "../../../engine/frame-builder/EngineFrameBuilder.h"
#include "../../../engine/frame-builder/EngineFrame.h"
#include "../../../engine/dto/DescriptorInstance.h"
#include "../../../engine/resource/FrameBufferInstance.h"
#include "../../service/PickingService.h"
#include "../../enum/engine-definitions.h"
#include "../../enum/EngineResourceIDs.h"
#include "../../../engine/dto/GlobalDataUBO.h"
#include "../../../engine/dto/LightData.h"
#include "../../../engine/dto/VolumeData.h"
#include "../../../engine/dto/MeshMetadata.h"
#include "ViewportHeaderPanel.h"
#include "ImGuizmo.h"
#include <algorithm>

namespace Metal {
    void EngineFramePanel::onInitialize() {
        const auto gBufferW = applicationContext->vulkanContext.getWindowWidth() / applicationContext->engineRepository.shadingResInvScale;
        const auto gBufferH = applicationContext->vulkanContext.getWindowHeight() / applicationContext->engineRepository.shadingResInvScale;

        engineFrame = EngineFrameBuilder()
                .addBuffer(RID_GLOBAL_DATA, sizeof(GlobalDataUBO),
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, UNIFORM_BUFFER)
                .addBuffer(RID_LIGHT_BUFFER, MAX_LIGHTS * sizeof(LightData),
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, STORAGE_BUFFER)
        .addBuffer(RID_VOLUMES_BUFFER, MAX_VOLUMES * sizeof(VolumeData),
                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, STORAGE_BUFFER)
                .addBuffer(RID_MESH_METADATA_BUFFER, MAX_MESH_INSTANCES * sizeof(MeshMetadata),
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, STORAGE_BUFFER)
                .addTexture(RID_ACCUMULATED_FRAME, gBufferW, gBufferH)
                .addTexture(RID_GBUFFER_POSITION_INDEX, gBufferW, gBufferH, VK_FORMAT_R32G32B32A32_SFLOAT)
                .addTexture(RID_GBUFFER_NORMAL, gBufferW, gBufferH, VK_FORMAT_R16G16B16A16_SFLOAT)
                .addTexture(RID_PREVIOUS_POSITION_INDEX, gBufferW, gBufferH, VK_FORMAT_R32G32B32_SFLOAT) // I noticed current position is rgba32f, but let's check.
                .addTexture(RID_PREVIOUS_NORMAL, gBufferW, gBufferH, VK_FORMAT_R16G16B16A16_SFLOAT)
                .addTexture(RID_DENOISED_FRAME, gBufferW, gBufferH, VK_FORMAT_R16G16B16A16_SFLOAT)
                .addTexture(RID_TEMPORAL_OUTPUT, gBufferW, gBufferH, VK_FORMAT_R16G16B16A16_SFLOAT)
                .addTexture(RID_PREVIOUS_DENOISED_FRAME, gBufferW, gBufferH, VK_FORMAT_R16G16B16A16_SFLOAT)
                .addFramebuffer(RID_SELECTION_FBO, gBufferW, gBufferH, glm::vec4(0, 0, 0, 0))
                .addColor(VK_FORMAT_R16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
                .addFramebuffer(RID_POST_PROCESSING_FBO, gBufferW, gBufferH, glm::vec4(0, 0, 0, 0))
                .addColor(VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
                .addComputeCommandBuffer(RID_COMPUTE_CB)
                .addPass(RAY_TRACING, RID_COMPUTE_CB)
                .addPass(ACCUMULATION, RID_COMPUTE_CB)
                .addPass(TEMPORAL_ACCUMULATION, RID_COMPUTE_CB)
                .addPass(SPATIAL_FILTER, RID_COMPUTE_CB)
                .addCommandBuffer(RID_SELECTION_CB, RID_SELECTION_FBO)
                .addPass(SELECTION_ID, RID_SELECTION_CB)
                .addCommandBuffer(RID_POST_PROCESSING_CB, RID_POST_PROCESSING_FBO)
                .addPass(POST_PROCESSING, RID_POST_PROCESSING_CB)
                .addPass(SELECTION_OUTLINE, RID_POST_PROCESSING_CB)
                .addPass(GRID, RID_POST_PROCESSING_CB)
                .addPass(ICONS, RID_POST_PROCESSING_CB)
                .build();

        applicationContext->engineContext.registerFrame(engineFrame.get());
    }

    void EngineFramePanel::onSync() {
        engineFrame->setShouldRender(true);
        const ImVec2 viewportSize = ImGui::GetContentRegionAvail();

        auto *framebuffer = engineFrame->getResourceAs<FrameBufferInstance>(RID_POST_PROCESSING_FBO);
        if (framebuffer) {
            applicationContext->descriptorSetService.setImageDescriptor(framebuffer, 0);
            ImGui::Image(reinterpret_cast<ImTextureID>(framebuffer->attachments[0]->imageDescriptor->vkDescriptorSet),
                         viewportSize);

            const ImVec2 imageMin = ImGui::GetItemRectMin();
            const ImVec2 imageMax = ImGui::GetItemRectMax();
            handleViewportPicking(imageMin, imageMax);
        }
    }

    void EngineFramePanel::handleViewportPicking(const ImVec2 &imageMin, const ImVec2 &imageMax) const {
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

        auto *gBufferPositionIndex = engineFrame->getResourceAs<TextureInstance>(RID_GBUFFER_POSITION_INDEX);
        if (!gBufferPositionIndex) {
            return;
        }

        const auto width = gBufferPositionIndex->width;
        const auto height = gBufferPositionIndex->height;
        const uint32_t pixelX = std::min(static_cast<uint32_t>(u * static_cast<float>(width)), width - 1);
        const uint32_t pixelY = std::min(static_cast<uint32_t>(v * static_cast<float>(height)), height - 1);

        const auto picked = applicationContext->pickingService.pickEntityFromGBuffer(gBufferPositionIndex, pixelX, pixelY);
        applicationContext->selectionService.clearSelection();
        applicationContext->selectionService.addSelected(picked.value_or(EMPTY_ENTITY));
    }
} // Metal
