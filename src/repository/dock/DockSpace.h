#ifndef METAL_ENGINE_DOCKSPACE_H
#define METAL_ENGINE_DOCKSPACE_H

#include <functional>
#include <string>

namespace Metal {
    class AbstractDockPanel;

    struct DockSpace {
        static DockSpace VIEWPORT;
        static DockSpace INSPECTOR;
        static DockSpace HIERARCHY;
        static DockSpace CONSOLE;
        static DockSpace FILES;
        static DockSpace METRICS;
        static const char *OPTIONS;

        const int index;
        const std::string name;
        const std::string icon;
        const int paddingX;
        const int paddingY;
        const std::function<AbstractDockPanel*()> getPanel;

        explicit DockSpace(const int index, std::string name, std::string icon,
                           const int paddingX, const int paddingY,
                           std::function<AbstractDockPanel *()> getPanel) : index(index),
                                                                            name(std::move(name)),
                                                                            icon(std::move(icon)),
                                                                            paddingX(paddingX),
                                                                            paddingY(paddingY),
                                                                            getPanel(std::move(getPanel)) {
        }

        static DockSpace *GetOption(int selected);
    };
}
#endif
