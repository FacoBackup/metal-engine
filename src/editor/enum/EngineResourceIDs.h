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
    inline constexpr int RID_GBUFFER_MOTION_VECTOR = 4;

    inline constexpr const char *RID_ACCUMULATED_FRAME = "accumulatedFrame";
    inline constexpr const char *RID_POST_PROCESSING_RT = "postProcessingRT";
    inline constexpr const char *RID_RT_COMPUTE_CB = "ComputeCB";
    inline constexpr const char *RID_POST_PROCESSING_CB = "PostProcessingCB";
    inline constexpr const char *RID_SELECTION_RT = "selectionRT";
    inline constexpr const char *RID_SELECTION_CB = "selectionCB";
    inline constexpr const char *RID_GBUFFER_CB = "gBufferCB";
    inline constexpr const char *RID_GBUFFER_RT = "gBufferRT";
    inline constexpr const char *RID_DOF_RT = "dofRT";
    inline constexpr const char *RID_DOF_CB = "dofCB";
    inline constexpr const char *RID_MOTION_BLUR_RT = "motionBlurRT";
    inline constexpr const char *RID_MOTION_BLUR_CB = "motionBlurCB";

    inline constexpr const char *RID_MOTION_VECTORS_RT = "motionVectorsRT";
    inline constexpr const char *RID_MOTION_VECTORS_CB = "motionVectorsCB";

    inline constexpr const char *RID_RESTIR_RESERVOIR_0 = "restirReservoir0";
    inline constexpr const char *RID_RESTIR_RESERVOIR_1 = "restirReservoir1";
}

#endif //ENGINE_RESOURCE_IDS_H
