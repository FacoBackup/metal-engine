#include "MaterialInstance.h"

namespace Metal {
    void MaterialInstance::dispose() {
        descriptorAlbedoTexture->dispose();
        descriptorNormalTexture->dispose();
        descriptorRoughnessTexture->dispose();
        descriptorMetallicTexture->dispose();
        descriptorHeightTexture->dispose();
    }
} // Metal
