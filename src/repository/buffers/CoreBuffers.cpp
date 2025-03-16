#include "CoreBuffers.h"

#include "../../dto/buffers/GlobalDataUBO.h"
#include "../../dto/buffers/TileInfoUBO.h"
#include "../../dto/buffers/LightData.h"
#include "../../enum/engine-definitions.h"
#include "../../service/buffer/BufferService.h"
#include "../../dto/buffers/RTTriangle.h"
#include "../../dto/buffers/BLAS.h"
#include "../../dto/buffers/MaterialInfo.h"
#include "../../dto/buffers/TLAS.h"

namespace Metal {
    void CoreBuffers::onInitialize() {
        globalData = bufferService.createBuffer(sizeof(GlobalDataUBO),
                                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        lightsBuffer = bufferService.createBuffer(MAX_LIGHT_VOLUMES * sizeof(LightData),
                                                  VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        volumesBuffer = bufferService.createBuffer(MAX_LIGHT_VOLUMES * sizeof(LightData),
                                                  VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        rtTrianglesBuffer = bufferService.createBuffer(MAX_RT_TRIANGLES * sizeof(RTTriangle),
                                                       VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        rtBLASBuffer = bufferService.createBuffer(MAX_RT_BLAS * sizeof(BLAS),
                                                  VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        rtTLASBuffer = bufferService.createBuffer(MAX_RT_TLAS * sizeof(TLAS),
                                                  VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        rtMaterialData = bufferService.createBuffer(MAX_MATERIALS * sizeof(MaterialInfo),
                                                    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }
}
