#ifndef INSPECTIONTOOLPROVIDER_H
#define INSPECTIONTOOLPROVIDER_H

#include "../dto/IToolProvider.h"

namespace Metal {
    struct WorldRepository;

    class InspectionToolProvider final : public IToolProvider {
        WorldRepository *worldRepository = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"WorldRepository", &worldRepository}
            };
        }

    protected:
        void registerTools() override;
    };
}

#endif
