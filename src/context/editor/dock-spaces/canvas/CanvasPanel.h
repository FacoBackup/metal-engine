#ifndef CANVASPANEL_H
#define CANVASPANEL_H
#include "../../../../dto/scripting/ScriptInstance.h"
#include "../docks/AbstractDockPanel.h"

namespace Metal {
    class CanvasPanel final : public AbstractDockPanel {
        ScriptInstance scriptInstance;

        static void RenderNodeIO(AbstractNode *nodeData, const ImVec2 &nodeWindowPos, ScriptInstance &scriptInstance);

        void handleNodeDrag(std::shared_ptr<AbstractNode> &nodeData);

        void renderNodes(ImVec2 pos);

    public:
        void onInitialize() override;


        void onSync() override;
    };
}

#endif // CANVASPANEL_H
