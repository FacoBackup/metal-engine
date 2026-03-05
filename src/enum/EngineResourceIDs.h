#ifndef ENGINE_RESOURCE_IDS_H
#define ENGINE_RESOURCE_IDS_H

namespace Metal {
    inline constexpr const char* RID_GLOBAL_DATA = "globalData";
    inline constexpr const char* RID_LIGHT_BUFFER = "lightBuffer";
    inline constexpr const char* RID_MESH_METADATA_BUFFER = "meshMetadataBuffer";
    inline constexpr const char* RID_GBUFFER_POSITION_INDEX = "gBufferPositionIndex";
    inline constexpr const char* RID_GBUFFER_NORMAL = "gBufferNormal";
    inline constexpr const char* RID_PREVIOUS_COLOR = "previousColor";
    inline constexpr const char* RID_PREVIOUS_POSITION_INDEX = "previousPositionIndex";
    inline constexpr const char* RID_PREVIOUS_NORMAL = "previousNormal";
    inline constexpr const char* RID_RAW_RENDERED_FRAME = "rawRenderedFrame";
    inline constexpr const char* RID_ACCUMULATED_FRAME = "accumulatedFrame";
    inline constexpr const char* RID_DENOISED_FRAME = "denoisedFrame";
    inline constexpr const char* RID_TEMPORAL_OUTPUT = "temporalOutput";
    inline constexpr const char* RID_PREVIOUS_DENOISED_FRAME = "previousDenoisedFrame";
    inline constexpr const char* RID_POST_PROCESSING_FBO = "postProcessingFBO";
    inline constexpr const char* RID_COMPUTE_CB = "ComputeCB";
    inline constexpr const char* RID_POST_PROCESSING_CB = "PostProcessingCB";
    inline constexpr const char* RID_ATMOSPHERE_DATA = "atmosphereData";
    inline constexpr const char* RID_VOLUMES_BUFFER = "volumesBuffer";
}

#endif //ENGINE_RESOURCE_IDS_H
