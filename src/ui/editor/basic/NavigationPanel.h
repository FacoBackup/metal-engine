#ifndef NAVIGATIONPANEL_H
#define NAVIGATIONPANEL_H
#include <unordered_map>

#include "../../data/NavigationPosition.h"
#include "../../repository/EditorRepository.h"
#include "../abstract/AbstractPanel.h"

namespace Metal {
    struct PanelInstance final {
        ViewInstance *instance;
        AbstractPanel *panel;
    };

    class NavigationPanel final : public AbstractPanel {
        NavigationPosition position;
        AbstractPanel *buttonsPanel = nullptr;
        std::array<PanelInstance, 2> panels{};

    public:
        explicit NavigationPanel(NavigationPosition position): position(position) {
        }

        void onInitialize() override;

        unsigned int getDefaultView();

        void onSync() override;
    };
} // Metal

#endif //NAVIGATIONPANEL_H
