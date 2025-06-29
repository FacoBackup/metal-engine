#ifndef MATERIALSERVICE_H
#define MATERIALSERVICE_H
#include "../../common/AbstractResourceService.h"

namespace Metal {
    struct MaterialInfo;

    class MaterialService final : public AbstractResourceService {
    public:

        MaterialInfo *create(const std::string &id);

        MaterialInfo *stream(const std::string &id) const;
    };
} // Metal

#endif //MATERIALSERVICE_H
