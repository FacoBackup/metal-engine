#ifndef METAL_ENGINE_DOCKSERVICE_H
#define METAL_ENGINE_DOCKSERVICE_H

#include "../../common/runtime/AbstractRuntimeComponent.h"
#include "../common/AbstractPanel.h"

namespace Metal {

    class DockService : public AbstractRuntimeComponent {
        bool isInitialized;
    public:
        void buildViews(ImGuiID windowId, Metal::AbstractPanel *panel);

        explicit DockService(ApplicationContext &context);
    };

} // Metal

#endif
