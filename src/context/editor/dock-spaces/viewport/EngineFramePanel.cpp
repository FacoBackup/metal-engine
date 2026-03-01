#include "EngineFramePanel.h"
#include "../../../../context/ApplicationContext.h"
#include "ViewportPanel.h"
#include "../../../../context/engine/frame-builder/EngineFrameBuilder.h"
#include "../../../../context/engine/frame-builder/EngineFrame.h"
#include "../../../../service/framebuffer/FrameBufferService.h"
#include "../../../../service/descriptor/DescriptorInstance.h"
#include "../../../../service/framebuffer/FrameBufferInstance.h"
#include "../../../../service/picking/PickingService.h"
#include "../../../../enum/engine-definitions.h"
#include "../../../../enum/EngineResourceIDs.h"
#include "../../../../dto/buffers/GlobalDataUBO.h"
#include "../../../../dto/buffers/TileInfoUBO.h"
#include "../../../../dto/buffers/LightData.h"
#include "../../../../dto/buffers/VolumeData.h"
#include "../../../../dto/buffers/MaterialData.h"
#include "../../../../dto/buffers/MeshMetadata.h"
#include "ViewportHeaderPanel.h"
#include "ImGuizmo.h"
#include <algorithm>

namespace Metal {
    void EngineFramePanel::onInitialize() {
        const auto gBufferW = CTX.vulkanContext.getWindowWidth() / CTX.engineRepository.shadingResInvScale;
        const auto gBufferH = CTX.vulkanContext.getWindowHeight() / CTX.engineRepository.shadingResInvScale;

        engineFrame = EngineFrameBuilder()
                .addBuffer(RID_GLOBAL_DATA, sizeof(GlobalDataUBO),
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, UNIFORM_BUFFER)
                .addBuffer(RID_TILE_INFO, sizeof(TileInfoUBO),
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, UNIFORM_BUFFER)
                .addBuffer(RID_LIGHT_BUFFER, MAX_LIGHTS * sizeof(LightData),
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, STORAGE_BUFFER)
                .addBuffer(RID_VOLUMES_BUFFER, MAX_VOLUMES * sizeof(VolumeData),
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, STORAGE_BUFFER)
                .addBuffer(RID_MATERIAL_BUFFER, MAX_MATERIALS * sizeof(MaterialData),
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, STORAGE_BUFFER)
                .addBuffer(RID_MESH_METADATA_BUFFER, MAX_MESH_INSTANCES * sizeof(MeshMetadata),
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, STORAGE_BUFFER)
                .addTexture(RID_SURFACE_CACHE, SURFACE_CACHE_RES, SURFACE_CACHE_RES)
                .addTexture(RID_RAW_RENDERED_FRAME, gBufferW, gBufferH)
                .addTexture(RID_ACCUMULATED_FRAME, gBufferW, gBufferH)
                .addTexture(RID_RENDER_INDEX_STENCIL, gBufferW, gBufferH, VK_FORMAT_R32_SFLOAT)
                .addFramebuffer(RID_POST_PROCESSING_FBO, CTX.vulkanContext.getWindowWidth(),
                                CTX.vulkanContext.getWindowHeight(), glm::vec4(0, 0, 0, 0))
                .addColor("Color", VK_FORMAT_R16G16B16A16_SFLOAT,
                          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, nullptr)
                .addComputeCommandBuffer(RID_COMPUTE_CB)
                .addPass(COMPUTE, RID_COMPUTE_CB)
                .addPass(ACCUMULATION, RID_COMPUTE_CB)
                .addCommandBuffer(RID_POST_PROCESSING_CB, RID_POST_PROCESSING_FBO)
                .addPass(POST_PROCESSING, RID_POST_PROCESSING_CB)
                .addPass(SELECTED_DOT, RID_POST_PROCESSING_CB)
                .addPass(GRID, RID_POST_PROCESSING_CB)
                .addPass(ICONS, RID_POST_PROCESSING_CB)
                .build();

        CTX.engineContext.registerFrame(engineFrame.get());
    }

    void EngineFramePanel::onSync() {
        engineFrame->setShouldRender(true);

        const float tabHeight = ImGui::GetFrameHeightWithSpacing();
        // Assuming this panel is inside ViewportPanel, we need to get its size.
        // But for now, let's use ImGui::GetContentRegionAvail() or similar if we can't access parent size easily.
        // Actually, ViewportPanel sets its size in updateInputs.
        const ImVec2 viewportSize = ImGui::GetContentRegionAvail();

        auto *framebuffer = engineFrame->getResourceAs<FrameBufferInstance>(RID_POST_PROCESSING_FBO);
        if (framebuffer) {
            CTX.descriptorSetService.setImageDescriptor(framebuffer, 0);
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

        auto *rawRenderedFrame = engineFrame->getResourceAs<TextureInstance>(RID_RAW_RENDERED_FRAME);
        if (!rawRenderedFrame) {
            return;
        }

        const auto width = rawRenderedFrame->width;
        const auto height = rawRenderedFrame->height;
        const uint32_t pixelX = std::min(static_cast<uint32_t>(u * static_cast<float>(width)), width - 1);
        const uint32_t pixelY = std::min(static_cast<uint32_t>(v * static_cast<float>(height)), height - 1);

        auto *renderIndexStencil = engineFrame->getResourceAs<TextureInstance>(RID_RENDER_INDEX_STENCIL);
        if (!renderIndexStencil) {
            return;
        }

        const auto picked = CTX.pickingService.pickEntityFromGBuffer(renderIndexStencil, pixelX, pixelY);
        CTX.selectionService.clearSelection();
        CTX.selectionService.addSelected(picked.value_or(EMPTY_ENTITY));
    }
} // Metal
