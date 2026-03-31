#ifndef METAL_ENGINE_LEVELSERVICE_H
#define METAL_ENGINE_LEVELSERVICE_H

#include "common/IService.h"
#include "common/ILoader.h"
#include <vector>

namespace Metal {
    class WorldRepository;

    class LevelService final : public IService, public ILoader {
        WorldRepository *worldRepository = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"WorldRepository", &worldRepository}
            };
        }

        void load(const std::string &absolutePath) override;

        bool isCompatible(const std::string &absolutePath) override;
    };
}

#endif //METAL_ENGINE_LEVELSERVICE_H
