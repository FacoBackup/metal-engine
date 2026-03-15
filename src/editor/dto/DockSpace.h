#ifndef METAL_ENGINE_DOCKSPACE_H
#define METAL_ENGINE_DOCKSPACE_H

#include <memory>
#include <vector>
#include <functional>
#include <string>

namespace Metal {
    class AbstractDockPanel;

    struct DockSpace {
        static DockSpace VIEWPORT;
        static DockSpace INSPECTOR;
        static DockSpace WORLD;
        static DockSpace CONSOLE;
        static DockSpace FILES;
        static DockSpace METRICS;
        static DockSpace REPOSITORIES;

        static const std::vector<DockSpace*> OPTIONS_LIST;

        const int index;
        const std::string name;
        const std::string icon;
        const int paddingX;
        const int paddingY;
        const std::function<std::shared_ptr<AbstractDockPanel>()> getPanel;

        explicit DockSpace(const int index, std::string name, std::string icon,
                           const int paddingX, const int paddingY,
                           std::function<std::shared_ptr<AbstractDockPanel> ()> getPanel) : index(index),
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
