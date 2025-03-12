#ifndef NODEPANEL_H
#define NODEPANEL_H
#include "../../../abstract/AbstractPanel.h"

namespace Metal {
    struct ScriptInstance;
    class IOPanel;
    struct AbstractDraggable;

    class NodePanel final : public AbstractPanel {
        AbstractDraggable *node = nullptr;
        ScriptInstance *scriptInstance = nullptr;

    public:
        void setNode(AbstractDraggable *node) {
            this->node = node;
        }

        void setScriptInstance(ScriptInstance *scriptInstance) {
            this->scriptInstance = scriptInstance;
        }

        void onSync() override;
    };
} // Metal

#endif //NODEPANEL_H
