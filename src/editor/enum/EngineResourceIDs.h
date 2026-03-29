#ifndef ENGINE_RESOURCE_IDS_H
#define ENGINE_RESOURCE_IDS_H

namespace Metal {
    inline constexpr const char *RID_GLOBAL_DATA = "globalData";
    inline constexpr const char *RID_LIGHT_BUFFER = "lightBuffer";
    inline constexpr const char *RID_MATERIAL_DATA_BUFFER = "materialDataBuffer";
    inline constexpr const char *RID_PRIMITIVE_DATA_BUFFER = "primitiveDataBuffer";

    inline constexpr int RID_GBUFFER_ALBEDO_EMISSIVE = 0;
    inline constexpr int RID_GBUFFER_ROUGHNESS_METALLIC = 1;
    inline constexpr int RID_GBUFFER_RENDER_INDEX_DEPTH = 2;
    inline constexpr int RID_GBUFFER_NORMAL = 3;

    inline constexpr const char *RID_ACCUMULATED_FRAME = "accumulatedFrame";
    inline constexpr const char *RID_POST_PROCESSING_FBO = "postProcessingFBO";
    inline constexpr const char *RID_RT_COMPUTE_CB = "ComputeCB";
    inline constexpr const char *RID_POST_PROCESSING_CB = "PostProcessingCB";
    inline constexpr const char *RID_SELECTION_FBO = "selectionFBO";
    inline constexpr const char *RID_SELECTION_CB = "selectionCB";
    inline constexpr const char *RID_GBUFFER_CB = "gBufferCB";
    inline constexpr const char *RID_GBUFFER_FBO = "gBufferFBO";
    inline constexpr const char *RID_DOF_FBO = "dofFBO";
    inline constexpr const char *RID_DOF_CB = "dofCB";
}

#endif //ENGINE_RESOURCE_IDS_H
