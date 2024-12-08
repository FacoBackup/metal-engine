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
    class RenderPassSystem final : public AbstractRuntimeComponent {
        std::array<AbstractRenderPass *, 2> renderPasses{};

    public:
        explicit RenderPassSystem(ApplicationContext &context) : AbstractRuntimeComponent(context) {
            renderPasses[0] = new VoxelVisualizerRenderPass(context);
        }

        void onSync() override;
    };
} // Metal

#endif
