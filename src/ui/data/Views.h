#ifndef METAL_ENGINE_DOCKSPACE_H
#define METAL_ENGINE_DOCKSPACE_H

#include <functional>
#include <string>

namespace Metal {
    class AbstractPanel;

    struct Views {
        static std::array<Views, 5> OPTIONS;

        static const std::string INSPECTOR;

        static const std::string HIERARCHY;

        static const std::string CONSOLE;

        static const std::string FILES;

        static const std::string METRICS;

        const std::string name;
        const std::string icon;
        const std::function<AbstractPanel*()> getPanel;

        explicit Views(std::string name, std::string icon,
                           std::function<AbstractPanel *()> getPanel) : name(std::move(name)),
                                                                            icon(std::move(icon)),
                                                                            getPanel(std::move(getPanel)) {
        }

        static unsigned int FindIndexByName(const std::string &name);
    };
}
#endif
