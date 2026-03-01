#ifndef ENGINE_RESOURCE_IDS_H
#define ENGINE_RESOURCE_IDS_H

namespace Metal {
    inline constexpr const char* RID_GLOBAL_DATA = "globalData";
    inline constexpr const char* RID_TILE_INFO = "tileInfo";
    inline constexpr const char* RID_LIGHT_BUFFER = "lightBuffer";
    inline constexpr const char* RID_VOLUMES_BUFFER = "volumesBuffer";
    inline constexpr const char* RID_MATERIAL_BUFFER = "materialBuffer";
    inline constexpr const char* RID_MESH_METADATA_BUFFER = "meshMetadataBuffer";
    inline constexpr const char* RID_SURFACE_CACHE = "surfaceCache";
    inline constexpr const char* RID_RENDER_INDEX_STENCIL = "renderIndexStencil";
    inline constexpr const char* RID_RAW_RENDERED_FRAME = "rawRenderedFrame";
    inline constexpr const char* RID_ACCUMULATED_FRAME = "accumulatedFrame";
    inline constexpr const char* RID_POST_PROCESSING_FBO = "postProcessingFBO";
    inline constexpr const char* RID_COMPUTE_CB = "ComputeCB";
    inline constexpr const char* RID_POST_PROCESSING_CB = "PostProcessingCB";
}

#endif //ENGINE_RESOURCE_IDS_H
