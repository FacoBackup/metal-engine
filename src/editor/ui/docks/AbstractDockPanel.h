#ifndef METAL_ENGINE_ABSTRACTDOCKPANEL_H
#define METAL_ENGINE_ABSTRACTDOCKPANEL_H

#include <glm/vec2.hpp>
#include <vector>

#include "editor/ui/abstract/AbstractPanel.h"
#include "../../dto/ShortcutDTO.h"

struct ImVec2;

namespace Metal {
    struct DockSpace;

    class AbstractDockPanel : public AbstractPanel {
    public:
        bool isWindowFocused = false;
        ImVec2 *position = nullptr;
        glm::vec2 *size = nullptr;
        DockSpace *dock = nullptr;

        std::vector<ShortcutDTO> shortcuts;

        const std::vector<ShortcutDTO> &getShortcuts() {
            return shortcuts;
        }
    };
}

#endif
