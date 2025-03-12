#ifndef IOPANEL_H
#define IOPANEL_H
#include <imgui.h>

#include "../../../abstract/AbstractPanel.h"

namespace Metal {
    struct ScriptInstance;
}

namespace Metal {
    struct IO;
    struct AbstractDraggable;

    class IOPanel final : public AbstractPanel {
        AbstractDraggable *node = nullptr;
        ScriptInstance *scriptInstance = nullptr;
        IO *draggingOutput = nullptr; // currently dragged output IO
        ImVec2 draggingOrigin; // screen position of the dragging output circle
        IO *hoveredInput = nullptr; // current drop target (input IO)
    public:
        void setNode(AbstractDraggable *node) {
            this->node = node;
        }

        void setScriptInstance(ScriptInstance *scriptInstance) {
            this->scriptInstance = scriptInstance;
        }

        void onSync() override;

        void onAfterSync();
    };
} // Metal

#endif //IOPANEL_H
