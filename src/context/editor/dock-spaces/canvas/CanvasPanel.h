#ifndef CANVASPANEL_H
#define CANVASPANEL_H
#include "../../../../dto/scripting/ScriptInstance.h"
#include "../docks/AbstractDockPanel.h"


namespace Metal {
    class LinkPanel;
    class CanvasHeaderPanel;
    class NodePanel;
    class IOPanel;

    class CanvasPanel final : public AbstractDockPanel {
        ScriptInstance scriptInstance;
        NodePanel *nodePanel = nullptr;
        LinkPanel *linkPanel = nullptr;
        IOPanel *ioPanel = nullptr;
        CanvasHeaderPanel *header = nullptr;
        ImVec2 spacing{50, 50};

        ImVec2 canvasOffset{0, 0};
        bool isDragging = false;

    public:
        void onInitialize() override;

        void drawBackground() const;

        void renderNodes();

        void renderLinks();

        void renderVirtualCanvas();

        void handleCanvasDrag();

        void onSync() override;
    };
}

#endif // CANVASPANEL_H
