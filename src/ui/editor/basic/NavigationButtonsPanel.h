#ifndef NAVIGATIONBUTTONSPANEL_H
#define NAVIGATIONBUTTONSPANEL_H
#include "../../data/NavigationPosition.h"
#include "../abstract/AbstractPanel.h"

namespace Metal {

class NavigationButtonsPanel final : public AbstractPanel{
    NavigationPosition position;
public:
    explicit NavigationButtonsPanel(NavigationPosition position): position(position) {
    }
    void onSync() override;
};

} // Metal

#endif //NAVIGATIONBUTTONSPANEL_H
