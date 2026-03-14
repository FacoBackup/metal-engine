#ifndef SELECTIONSERVICE_H
#define SELECTIONSERVICE_H
#include <vector>

#include "../../common/IService.h"
#include "../enum/engine-definitions.h"

namespace Metal {
    struct WorldRepository;
    struct EditorRepository;

    class SelectionService final : public IService {
        EditorRepository *editorRepository = nullptr;
        WorldRepository *worldRepository = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"EditorRepository", editorRepository},
                {"WorldRepository", worldRepository}
            };
        }

        void addSelected(entt::entity entity) const;

        void clearSelection() const;

        void addAllSelected(const std::vector<entt::entity> &all) const;

        void updatePrimitiveSelected() const;
    };
} // Metal

#endif //SELECTIONSERVICE_H
