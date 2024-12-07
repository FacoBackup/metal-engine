#ifndef TEXTURESERVICE_H
#define TEXTURESERVICE_H
#include "TextureInstance.h"
#include "../AbstractResourceService.h"
#include "../../../common/interface/AbstractRuntimeComponent.h"

namespace Metal {
    class TextureService final : public AbstractResourceService {
    public:
        explicit TextureService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }

        [[nodiscard]] TextureInstance *createTextureInstance(int w, int h, VkFormat format, bool isLinear) const;
    };
} // Metal

#endif //TEXTURESERVICE_H
