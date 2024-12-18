#ifndef METAL_ENGINE_ABSTRACTDOCKPANEL_H
#define METAL_ENGINE_ABSTRACTDOCKPANEL_H

#include <glm/vec2.hpp>

#include "../../common/AbstractPanel.h"

struct ImVec2;

namespace Metal {
    struct DockDTO;

    class AbstractDockPanel : public AbstractPanel {
    public:
        bool isWindowFocused = false;
        ImVec2 *position = nullptr;
        glm::vec2 *size = nullptr;
        DockDTO *dock = nullptr;
    };
}

#endif
