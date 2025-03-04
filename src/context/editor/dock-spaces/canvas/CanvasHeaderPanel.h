#ifndef CANVASHEADERPANEL_H
#define CANVASHEADERPANEL_H
#include "../../abstract/AbstractPanel.h"

namespace Metal {
    struct ScriptInstance;
}

namespace Metal {
    class CanvasHeaderPanel final : public AbstractPanel {
        ScriptInstance* script = nullptr;
    public:
        explicit CanvasHeaderPanel(ScriptInstance *script);
        void onSync() override;
    };
} // Metal

#endif //CANVASHEADERPANEL_H
