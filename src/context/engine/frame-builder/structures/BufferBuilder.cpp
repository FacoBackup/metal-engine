#include "BufferBuilder.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/buffer/BufferInstance.h"
#include "../../../../service/buffer/BufferService.h"

namespace Metal {
    ResourceType BufferBuilder::getType() {
        return ResourceType::BUFFER;
    }

    RuntimeResource *BufferBuilder::build() {
        auto *buffer = CTX.bufferService.createBuffer(id, size, usage, properties);
        buffer->setAsNoDisposal();

        return buffer;
    }
} // Metal
