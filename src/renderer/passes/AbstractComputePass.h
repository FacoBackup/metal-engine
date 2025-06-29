#ifndef ABSTRACTCOMPUTEPASS_H
#define ABSTRACTCOMPUTEPASS_H
#include <glm/vec4.hpp>

#include "AbstractPass.h"

namespace Metal {
    struct WorldRepository;
    class StreamingService;
    struct CoreFrameBuffers;
    struct PipelineInstance;
    struct CoreBuffers;
    struct TextureInstance;

    class AbstractComputePass : public AbstractPass {
    public:
        explicit AbstractComputePass();

        void recordDispatch(unsigned int groupX, unsigned int groupY, unsigned int groupZ) const;

        void clearTexture(const VkImage &image, glm::vec4 clearColor = {0, 0, 0, 1}) const;

        void recordImageDispatch(const TextureInstance *image, unsigned int threadCountX,
                                 unsigned int threadCountY) const;

        void endWriting(VkImage &vkImage) const;

        void startWriting(VkImage &vkImage) const;
    };
}


#endif //ABSTRACTCOMPUTEPASS_H
