#ifndef METAL_ENGINE_DOCKSPACE_H
#define METAL_ENGINE_DOCKSPACE_H

#include <functional>
#include <string>

namespace Metal {
    class AbstractPanel;

    struct DockSpace {
        static const DockSpace VIEWPORT;
        static const DockSpace INSPECTOR;
        static const DockSpace HIERARCHY;
        static const DockSpace CONSOLE;
        static const DockSpace FILES;
        static const DockSpace RESOURCES;
        static const DockSpace METRICS;

        const int index;
        const std::string name;
        const std::string icon;
        const int paddingX;
        const int paddingY;
        const std::function<std::unique_ptr<AbstractPanel>()> getPanel;

        explicit DockSpace(int index, std::string name, std::string icon,
                           int paddingX, int paddingY,
                           std::function<std::unique_ptr<AbstractPanel>()> getPanel) : index(index),
                                                                                       name(std::move(name)),
                                                                                       icon(std::move(icon)),
                                                                                       paddingX(paddingX),
                                                                                       paddingY(paddingY),
                                                                                       getPanel(std::move(getPanel)) {}
    };
}
#endif
