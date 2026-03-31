#ifndef METRICSPANEL_H
#define METRICSPANEL_H

#include "../docks/AbstractDockPanel.h"
#include "imgui.h"

namespace Metal {
    class MeshService;
    class VoxelService;
    class RenderTargetService;
    class TextureService;
    class PipelineService;
    class BufferService;

    class MetricsPanel final : public AbstractDockPanel {
        MeshService *meshService = nullptr;
        VoxelService *voxelService = nullptr;
        RenderTargetService *RenderTargetService = nullptr;
        TextureService *textureService = nullptr;
        PipelineService *pipelineService = nullptr;
        BufferService *bufferService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"MeshService", &meshService},
                {"VoxelService", &voxelService},
                {"RenderTargetService", &RenderTargetService},
                {"TextureService", &textureService},
                {"PipelineService", &pipelineService},
                {"BufferService", &bufferService}
            };
        }

        void onSync() override;
    };
}

#endif
