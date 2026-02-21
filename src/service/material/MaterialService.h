#ifndef MATERIALSERVICE_H
#define MATERIALSERVICE_H
#include "MaterialFileData.h"
#include "MaterialInstance.h"
#include "../../service/abstract/AbstractResourceService.h"

#include "../../enum/engine-definitions.h"
#include <vector>
#include "../../dto/buffers/MaterialData.h"

namespace Metal {
    class MaterialService final : public AbstractResourceService {
        unsigned int nextMaterialIndex = 1;
        std::vector<MaterialData> materials {MAX_MATERIALS};
    public:
        explicit MaterialService()
            : AbstractResourceService() {
        }

        MaterialInstance *create(const std::string &id);

        MaterialFileData *stream(const std::string &id) const;

        unsigned int getMaterialIndex(const std::string &id);
    };
} // Metal

#endif //MATERIALSERVICE_H
