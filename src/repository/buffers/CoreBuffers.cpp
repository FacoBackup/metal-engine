#include "CoreBuffers.h"

#include "../../context/ApplicationContext.h"
#include "../../dto/buffers/GlobalDataUBO.h"
#include "../../dto/buffers/TileInfoUBO.h"
#include "../../dto/buffers/LightData.h"
#include "../../dto/buffers/VolumeData.h"
#include "../../dto/buffers/MaterialData.h"
#include "../../enum/engine-definitions.h"
#include "../../service/buffer/BufferService.h"

namespace Metal {
    void CoreBuffers::onInitialize() {
        auto &bufferService = CTX.bufferService;
        globalData = bufferService.createBuffer(sizeof(GlobalDataUBO),
                                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        tileInfo = bufferService.createBuffer(sizeof(TileInfoUBO),
                                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        lightBuffer = bufferService.createBuffer(MAX_LIGHTS * sizeof(LightData),
                                                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        volumesBuffer = bufferService.createBuffer(MAX_VOLUMES * sizeof(VolumeData),
                                                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        materialBuffer = bufferService.createBuffer(MAX_MATERIALS * sizeof(MaterialFileData),
                                                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }
}
