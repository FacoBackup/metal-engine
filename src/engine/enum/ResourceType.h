#ifndef RESOURCETYPE_H
#define RESOURCETYPE_H

namespace Metal {
    enum ResourceType {
        FRAMEBUFFER,
        TEXTURE,
        PIPELINE,
        BUFFER,
        DESCRIPTOR_SET,
        COMMAND_BUFFER_RECORDER,
        BLAS_INSTANCE
    };
}

#endif //RESOURCETYPE_H
