#ifndef TEXTURESERVICE_H
#define TEXTURESERVICE_H
#include "../runtime/TextureInstance.h"
#include "../../common/interface/AbstractResourceService.h"

namespace Metal {
    class TextureService final : public AbstractResourceService {
    public:
        explicit TextureService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }
    };
} // Metal

#endif //TEXTURESERVICE_H
