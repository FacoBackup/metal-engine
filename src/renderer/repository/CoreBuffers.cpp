#include "CoreBuffers.h"

#include "../data/GlobalDataUBO.h"
#include "../data/LightBuffer.h"
#include "../../common/engine-definitions.h"
#include "../service/BufferService.h"
#include "../data/TriangleBuffer.h"
#include "../data/BLASBuffer.h"
#include "../data/MaterialInfo.h"
#include "../data/TLASBuffer.h"
#include "../../MetalContextProvider.h"

namespace Metal {
    void CoreBuffers::onInitialize() {
        globalData = SINGLETONS.bufferService.createBuffer(sizeof(GlobalDataUBO),
                                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        lightsBuffer = SINGLETONS.bufferService.createBuffer(MAX_LIGHT_VOLUMES * sizeof(LightBuffer),
                                                  VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        volumesBuffer = SINGLETONS.bufferService.createBuffer(MAX_LIGHT_VOLUMES * sizeof(LightBuffer),
                                                  VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        rtTrianglesBuffer = SINGLETONS.bufferService.createBuffer(MAX_RT_TRIANGLES * sizeof(TriangleBuffer),
                                                       VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        rtBLASBuffer = SINGLETONS.bufferService.createBuffer(MAX_RT_BLAS * sizeof(BLASBuffer),
                                                  VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        rtTLASBuffer = SINGLETONS.bufferService.createBuffer(MAX_RT_TLAS * sizeof(TLASBuffer),
                                                  VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        rtMaterialData = SINGLETONS.bufferService.createBuffer(MAX_MATERIALS * sizeof(MaterialInfo),
                                                    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }
}
