#ifndef REPOSITORIESPANEL_H
#define REPOSITORIESPANEL_H

#include "../docks/AbstractDockPanel.h"

namespace Metal {
    class FormPanel;
    class Inspectable;

    class RepositoriesPanel final : public AbstractDockPanel {
        std::vector<Inspectable *> repositories{};
        FormPanel *formPanel = nullptr;

    public:
        void onInitialize() override;
        void onSync() override;
    };
}

#endif
