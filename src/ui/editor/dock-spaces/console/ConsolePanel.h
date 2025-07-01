#ifndef CONSOLEPANEL_H
#define CONSOLEPANEL_H
#include "../../abstract/AbstractPanel.h"

namespace Metal
{
    class ConsolePanel final : public AbstractPanel
    {
        bool showInfo = true;
        bool showWarning = true;
        bool showError = true;
        char searchBuffer[256] = "";

    public:
        void onSync() override;
    };
}

#endif
