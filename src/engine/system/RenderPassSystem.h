#ifndef RENDERPASSSYSTEM_H
#define RENDERPASSSYSTEM_H
#include <array>

#include "../render-pass/AbstractRenderPass.h"
#include "../render-pass/impl/VoxelVisualizerRenderPass.h"

namespace Metal {
    class ApplicationContext;
    /**
     * Executes 'onSync' for each render pass and manages all the render passes
     */
    class RenderPassSystem final {
        std::array<AbstractRenderPass *, 2> renderPasses = {new VoxelVisualizerRenderPass(context), nullptr};
        ApplicationContext &context;

    public:
        explicit RenderPassSystem(ApplicationContext &context) : context(context) {
        }

        void render(VulkanFrameData &frameData);
    };
} // Metal

#endif
