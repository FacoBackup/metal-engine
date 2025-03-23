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
        static DockSpace CANVAS;
        static const char *OPTIONS;

        const unsigned int index;
        const std::string name;
        const std::string icon;
        const std::function<AbstractDockPanel*()> getPanel;

        explicit DockSpace(const int index, std::string name, std::string icon,
                           std::function<AbstractDockPanel *()> getPanel) : index(index),
                                                                            name(std::move(name)),
                                                                            icon(std::move(icon)),
                                                                            getPanel(std::move(getPanel)) {
        }

        static DockSpace *GetOption(unsigned int selected);

        static int GetOptionCount();
    };
}
#endif
