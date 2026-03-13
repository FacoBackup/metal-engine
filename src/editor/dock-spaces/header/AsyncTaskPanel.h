#ifndef METAL_ENGINE_ASYNCTASKPANEL_H
#define METAL_ENGINE_ASYNCTASKPANEL_H

#include "../../abstract/AbstractPanel.h"

namespace Metal {
    class AsyncTaskPanel final : public AbstractPanel {
    public:
        void onSync() override;
    };
}

#endif
