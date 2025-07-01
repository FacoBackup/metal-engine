#ifndef SELECTIONSERVICE_H
#define SELECTIONSERVICE_H
#include <vector>

#include "../../common/AbstractRuntimeComponent.h"
#include "../../common/engine-definitions.h"

namespace Metal {
    struct WorldRepository;
    struct EditorRepository;

    class SelectionService final : public AbstractRuntimeComponent {
    public:
        void addSelected(EntityID entity) const;

        void clearSelection() const;

        void addAllSelected(const std::vector<EntityID> &all) const;

        void updatePrimitiveSelected() const;
    };
} // Metal

#endif //SELECTIONSERVICE_H
