#include "MaterialInstance.h"

namespace Metal {
    void MaterialInstance::dispose(VulkanContext &context) {
        descriptorAlbedoTexture->dispose(context);
        descriptorNormalTexture->dispose(context);
        descriptorRoughnessTexture->dispose(context);
        descriptorMetallicTexture->dispose(context);
        descriptorHeightTexture->dispose(context);
    }
} // Metal
