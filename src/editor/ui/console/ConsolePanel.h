#ifndef CONSOLEPANEL_H
#define CONSOLEPANEL_H
#include "../docks/AbstractDockPanel.h"

namespace Metal {
    class ConsolePanel final : public AbstractDockPanel {
        public:
        void onSync() override;
    };
}

#endif
