#ifndef MATERIALSERVICE_H
#define MATERIALSERVICE_H
#include "MaterialFileData.h"
#include "MaterialInstance.h"
#include "../../service/abstract/AbstractResourceService.h"
#include "../../service/abstract/IStreamable.h"

#include "../../enum/engine-definitions.h"
#include <vector>
#include "../../dto/buffers/MaterialData.h"

namespace Metal {
    class MaterialService final : public IStreamable<MaterialInstance> {
        unsigned int nextMaterialIndex = 1;
        std::vector<MaterialData> materials {MAX_MATERIALS};
    public:
        MaterialInstance *create(const std::string &id) override;

        MaterialFileData *stream(const std::string &id) const;

        unsigned int getMaterialIndex(const std::string &id);

        void disposeResource(MaterialInstance *resource) override;
    };
} // Metal

#endif //MATERIALSERVICE_H
