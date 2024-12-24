#ifndef INSPECTOR_H
#define INSPECTOR_H

#include "../../../../enum/engine-definitions.h"
#include "../docks/AbstractDockPanel.h"

namespace Metal {
    class FormPanel;
    class Inspectable;

    class InspectorPanel final : public AbstractDockPanel {
        std::vector<Inspectable *> repositories{};
        std::vector<Inspectable *> additionalInspection{};
        Inspectable *currentInspection = nullptr;
        Inspectable *selectedEntity = nullptr;

        FormPanel *formPanel = nullptr;
        EntityID selectedId = EMPTY_ENTITY;

    public:
        void onInitialize() override;

        void onSync() override;

        void tick();
    };
}

#endif