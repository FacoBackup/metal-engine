#include "BufferBuilder.h"
#include "../../../ApplicationContext.h"
#include "../../resource/BufferInstance.h"
#include "../../service/BufferService.h"

namespace Metal {
    ResourceType BufferBuilder::getType() {
        return BUFFER;
    }

    RuntimeResource *BufferBuilder::build() {
        auto *buffer = bufferService->getResource(id);
        if (buffer != nullptr) {
            return buffer;
        }

        buffer = bufferService->createBuffer(id, size, usage, properties, type);
        buffer->setAsNoDisposal();

        return buffer;
    }
} // Metal
