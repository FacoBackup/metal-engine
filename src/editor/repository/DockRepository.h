#ifndef DOCKREPOSITORY_H
#define DOCKREPOSITORY_H

#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "common/IRepository.h"
#include "../dto/DockDefinition.h"
#include "common/IInit.h"

namespace Metal {
    class DockRepository final : public IRepository, public IInit {
    public:
        void clear() override;

        void registerFields() override;

        void onInitialize() override;

        [[nodiscard]] std::shared_ptr<DockDefinition> getCenter() const { return center; }

        [[nodiscard]] std::vector<std::shared_ptr<DockDefinition> > &getBottom() { return bottom; }

        [[nodiscard]] const std::vector<std::shared_ptr<DockDefinition> > &getBottom() const { return bottom; }

        [[nodiscard]] std::vector<std::shared_ptr<DockDefinition> > &getLeft() { return left; }

        [[nodiscard]] const std::vector<std::shared_ptr<DockDefinition> > &getLeft() const { return left; }

        [[nodiscard]] std::vector<std::shared_ptr<DockDefinition> > &getRight() { return right; }

        [[nodiscard]] const std::vector<std::shared_ptr<DockDefinition> > &getRight() const { return right; }

    private:
        std::shared_ptr<DockDefinition> center;
        std::vector<std::shared_ptr<DockDefinition> > bottom;
        std::vector<std::shared_ptr<DockDefinition> > left;
        std::vector<std::shared_ptr<DockDefinition> > right;
    };
}

#endif //DOCKREPOSITORY_H
