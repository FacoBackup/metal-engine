#ifndef NAVIGATIONPANEL_H
#define NAVIGATIONPANEL_H
#include "../../data/NavigationPosition.h"
#include "../abstract/AbstractPanel.h"

namespace Metal {
    class NavigationPanel final : public AbstractPanel {
        NavigationPosition position;
        AbstractPanel *buttonsPanel = nullptr;
        AbstractPanel *selectedView = nullptr;
         int selectedLocal = -1;
    public:
        explicit NavigationPanel(NavigationPosition position): position(position) {
        }

        void onInitialize() override;

        unsigned int getDefaultView();

        void onSync() override;
    };
} // Metal

#endif //NAVIGATIONPANEL_H
