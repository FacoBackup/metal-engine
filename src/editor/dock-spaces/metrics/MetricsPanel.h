#ifndef METRICSPANEL_H
#define METRICSPANEL_H

#include "../docks/AbstractDockPanel.h"
#include "imgui.h"

namespace Metal {
    class MeshService;
    class VoxelService;
    class FrameBufferService;
    class TextureService;
    class PipelineService;
    class BufferService;

    class MetricsPanel final : public AbstractDockPanel {
        MeshService *meshService = nullptr;
        VoxelService *voxelService = nullptr;
        FrameBufferService *framebufferService = nullptr;
        TextureService *textureService = nullptr;
        PipelineService *pipelineService = nullptr;
        BufferService *bufferService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"MeshService", meshService},
                {"VoxelService", voxelService},
                {"FrameBufferService", framebufferService},
                {"TextureService", textureService},
                {"PipelineService", pipelineService},
                {"BufferService", bufferService}
            };
        }

        void onSync() override;
    };
}

#endif
