#ifndef DOCKREPOSITORY_H
#define DOCKREPOSITORY_H

#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "common/IRepository.h"
#include "../dto/DockDefinition.h"
#include "common/IInit.h"

namespace Metal {
    struct DockRepository final : IRepository, IInit {
        std::shared_ptr<DockDefinition> center;
        std::vector<std::shared_ptr<DockDefinition>> bottom;
        std::vector<std::shared_ptr<DockDefinition>> left;
        std::vector<std::shared_ptr<DockDefinition>> right;

        void onInitialize() override;

        [[nodiscard]] nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json &j) override;
    };
}

#endif //DOCKREPOSITORY_H
