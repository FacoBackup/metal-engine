#ifndef MATERIALSERVICE_H
#define MATERIALSERVICE_H
#include "MaterialData.h"
#include "MaterialInstance.h"
#include "../../service/abstract/AbstractResourceService.h"

namespace Metal {
    class MaterialService final : public AbstractResourceService {
    public:
        explicit MaterialService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }

        MaterialInstance *create(const std::string &id);

        MaterialData *stream(const std::string &id) const;
    };
} // Metal

#endif //MATERIALSERVICE_H
