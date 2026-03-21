#ifndef MCPPANEL_H
#define MCPPANEL_H

#include "../docks/AbstractDockPanel.h"
#include "imgui.h"

namespace Metal {
    class MCPService;

    class MCPPanel final : public AbstractDockPanel {
    public:
        void onSync() override;

        std::vector<Dependency> getDependencies() override {
            return {
                {"MCPService", &mcpService}
            };
        }

        explicit MCPPanel();

    private:
        MCPService *mcpService = nullptr;
        char inputBuffer[1024]{};
    };
}

#endif
