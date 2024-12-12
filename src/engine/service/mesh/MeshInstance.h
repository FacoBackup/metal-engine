#ifndef MESHINSTANCE_H
#define MESHINSTANCE_H
#include "../../../context/runtime/RuntimeResource.h"

namespace Metal {
    struct BufferInstance;

    class MeshInstance final : public RuntimeResource {
        BufferInstance *vertexBuffer = nullptr;
        BufferInstance *indexBuffer = nullptr;
        BufferInstance *normalBuffer = nullptr;

    public:
        void setVertexBuffer(BufferInstance *buffer) {
            vertexBuffer = buffer;
        }

        void setIndexBuffer(BufferInstance *buffer) {
            indexBuffer = buffer;
        }

        void setNormalBuffer(BufferInstance *buffer) {
            normalBuffer = buffer;
        }

        [[nodiscard]] BufferInstance *getVertexBuffer() const {
            return vertexBuffer;
        }

        [[nodiscard]] BufferInstance *getIndexBuffer() const {
            return indexBuffer;
        }

        [[nodiscard]] BufferInstance *getNormalBuffer() const {
            return normalBuffer;
        }

        void dispose(VulkanContext &context) override;
    };
} // Metal

#endif //MESHINSTANCE_H
