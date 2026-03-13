#ifndef COMMANDBUFFERRECORDERSERVICE_H
#define COMMANDBUFFERRECORDERSERVICE_H

#include "../../../common/AbstractResourceService.h"
#include "../passes/CommandBufferRecorder.h"

namespace Metal {
    class CommandBufferRecorderService final : public AbstractResourceService<CommandBufferRecorder> {
    public:
        void disposeResource(CommandBufferRecorder *resource) override {
        }
        
        CommandBufferRecorder* create(const std::string& id, FrameBufferInstance *frameBuffer, bool clearBuffer = true) {
            return createResourceInstance(id, frameBuffer, clearBuffer);
        }

        CommandBufferRecorder* createCompute(const std::string& id) {
            return createResourceInstance(id);
        }
    };
}

#endif
