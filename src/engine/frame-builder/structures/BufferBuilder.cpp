#include "BufferBuilder.h"
#include "../../../ApplicationContext.h"
#include "../../resource/BufferInstance.h"
#include "../../service/BufferService.h"

namespace Metal {
    ResourceType BufferBuilder::getType() {
        return ResourceType::BUFFER;
    }

    RuntimeResource *BufferBuilder::build() {
        auto *buffer = CTX.bufferService.getResource(id);
        if (buffer != nullptr) {
            return buffer;
        }

        buffer = CTX.bufferService.createBuffer(id, size, usage, properties, type);
        buffer->setAsNoDisposal();

        return buffer;
    }
} // Metal
