#ifndef MATERIALSERVICE_H
#define MATERIALSERVICE_H
#include "../../service/abstract/AbstractResourceService.h"

namespace Metal {
    struct MaterialInfo;

    class MaterialService final : public AbstractResourceService {
    public:
        explicit MaterialService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }

        MaterialInfo *create(const std::string &id);

        MaterialInfo *stream(const std::string &id) const;
    };
} // Metal

#endif //MATERIALSERVICE_H
