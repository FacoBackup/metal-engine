#include "CoreBuffers.h"

#include "../../dto/buffers/GlobalDataUBO.h"
#include "../../dto/buffers/TileInfoUBO.h"
#include "../../dto/buffers/LightVolumeData.h"
#include "../../enum/engine-definitions.h"
#include "../../service/buffer/BufferService.h"
#include "../../service/rt/data/BottomLevelAccelerationStructure.h"
#include "../../service/rt/data/RTTriangle.h"
#include "../../service/rt/data/TopLevelAccelerationStructure.h"

namespace Metal {
    void CoreBuffers::onInitialize() {
        globalData = bufferService.createBuffer(sizeof(GlobalDataUBO),
                                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        tileInfo = bufferService.createBuffer(sizeof(TileInfoUBO),
                                              VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        lightVolumeBuffer = bufferService.createBuffer(MAX_LIGHT_VOLUMES * sizeof(LightVolumeData),
                                                       VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);


        rtTrianglesBuffer = bufferService.createBuffer(MAX_RT_TRIANGLES * sizeof(RTTriangle),
                                                       VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        rtBLASBuffer = bufferService.createBuffer(MAX_RT_BLAS * sizeof(BottomLevelAccelerationStructure),
                                                  VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        rtTLASBuffer = bufferService.createBuffer(MAX_RT_TLAS * sizeof(TopLevelAccelerationStructure),
                                                  VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }
}
