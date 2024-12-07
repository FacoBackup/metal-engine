#ifndef TEXTURESERVICE_H
#define TEXTURESERVICE_H
#include "TextureInstance.h"
#include "../AbstractResourceService.h"

namespace Metal {
    class TextureService final : public AbstractResourceService {
    public:
        explicit TextureService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }
    };
} // Metal

#endif //TEXTURESERVICE_H
