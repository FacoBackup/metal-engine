#ifndef ABSTRACTCOMPUTEPASS_H
#define ABSTRACTCOMPUTEPASS_H
#include "../passes/AbstractPass.h"

namespace Metal {
    struct WorldRepository;
    struct MeshInstance;
    class StreamingRepository;
    struct CoreFrameBuffers;
    struct CorePipelines;
    struct PipelineInstance;
    struct CoreBuffers;
    struct TextureInstance;

    class AbstractComputePass : public AbstractPass {
    public:
        explicit AbstractComputePass(ApplicationContext &context);

        void recordDispatch(unsigned int groupX, unsigned int groupY, unsigned int groupZ) const;

        void recordImageDispatch(const TextureInstance *image, unsigned int threadCountX,
                                                      unsigned int threadCountY) const;
    };
}


#endif //ABSTRACTCOMPUTEPASS_H
