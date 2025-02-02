#include "CoreBuffers.h"

#include "../../dto/buffers/GlobalDataUBO.h"
#include "../../dto/buffers/TileInfoUBO.h"
#include "../../dto/buffers/LightVolumeData.h"
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

        lightVolumeBuffer = bufferService.createBuffer(MAX_LIGHT_VOLUMES * sizeof(LightVolumeData),
                                                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }
}
