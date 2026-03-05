#include "VolumeService.h"
#include "../../context/ApplicationContext.h"
#include "../../repository/world/components/VolumeComponent.h"
#include "../buffer/BufferInstance.h"
#include "../../enum/EngineResourceIDs.h"

namespace Metal {
    void VolumeService::registerVolumes() {
    }

    void VolumeService::onSync() {
        items.clear();

        registerVolumes();

        if (!items.empty()) {
            CTX.engineContext.currentFrame->getResourceAs<BufferInstance>(RID_VOLUMES_BUFFER)->update(items.data());
        }
    }
} // Metal
