#ifndef INSPECTOR_H
#define INSPECTOR_H

#include "../../enum/engine-definitions.h"
#include "../docks/AbstractDockPanel.h"

namespace Metal {
    class FormPanel;
    class Inspectable;
    struct EditorRepository;
    struct WorldRepository;

    class InspectorPanel final : public AbstractDockPanel {
        std::vector<Inspectable *> additionalInspection{};
        Inspectable *selectedEntity = nullptr;

        FormPanel *formPanel = nullptr;
        entt::entity selectedId = EMPTY_ENTITY;

        EditorRepository *editorRepository = nullptr;
        WorldRepository *worldRepository = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"EditorRepository", &editorRepository},
                {"WorldRepository", &worldRepository}
            };
        }

        void onInitialize() override;

        void onSync() override;

        void tick();
    };
}

#endif
