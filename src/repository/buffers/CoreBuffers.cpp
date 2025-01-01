#include "CoreBuffers.h"

#include "../../dto/ubo/GlobalDataUBO.h"
#include "../../dto/push-constant/PostProcessingPushConstant.h"
#include "../../dto/ubo/TileInfoUBO.h"
#include "../../dto/ubo/LightData.h"
#include "../../enum/engine-definitions.h"
#include "../../service/buffer/BufferService.h"

namespace Metal {
    void CoreBuffers::onInitialize() {
        globalData = bufferService.createBuffer(sizeof(GlobalDataUBO),
                                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        tileInfo = bufferService.createBuffer(sizeof(TileInfoUBO),
                                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        lights = bufferService.createBuffer(MAX_LIGHTS * sizeof(LightData),
                                                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }
}
