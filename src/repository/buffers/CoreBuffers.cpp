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
        globalData = bufferService.createBuffer("globalData", sizeof(GlobalDataUBO),
                                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                UNIFORM_BUFFER);

        tileInfo = bufferService.createBuffer("tileInfo", sizeof(TileInfoUBO),
                                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                UNIFORM_BUFFER);

        lightBuffer = bufferService.createBuffer("lightBuffer", MAX_LIGHTS * sizeof(LightData),
                                                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                STORAGE_BUFFER);

        volumesBuffer = bufferService.createBuffer("volumesBuffer", MAX_VOLUMES * sizeof(VolumeData),
                                                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                STORAGE_BUFFER);

        materialBuffer = bufferService.createBuffer("materialBuffer", MAX_MATERIALS * sizeof(MaterialData),
                                                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                STORAGE_BUFFER);
    }
}
