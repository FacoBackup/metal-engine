#ifndef SELECTIONSERVICE_H
#define SELECTIONSERVICE_H
#include <vector>

#include "../../common/AbstractRuntimeComponent.h"
#include "../../enum/engine-definitions.h"

namespace Metal {
    struct WorldRepository;
    struct EditorRepository;

    class SelectionService final : public AbstractRuntimeComponent {
        EditorRepository &editorRepository;
        WorldRepository &world;

    public:
        explicit SelectionService(ApplicationContext &context);

        void addSelected(EntityID entity) const;

        void clearSelection() const;

        void addAllSelected(const std::vector<EntityID> &all) const;

        void updatePrimitiveSelected() const;
    };
} // Metal

#endif //SELECTIONSERVICE_H
