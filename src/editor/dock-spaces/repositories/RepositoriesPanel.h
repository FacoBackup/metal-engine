#ifndef REPOSITORIESPANEL_H
#define REPOSITORIESPANEL_H

#include "../docks/AbstractDockPanel.h"

namespace Metal {
    class FormPanel;
    class Inspectable;
    struct EditorRepository;
    struct EngineRepository;
    struct WorldRepository;

    class RepositoriesPanel final : public AbstractDockPanel {
        std::vector<Inspectable *> repositories{};
        std::shared_ptr<FormPanel> formPanel = nullptr;

        WorldRepository *worldRepository = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"WorldRepository", &worldRepository}
            };
        }

        void onInitialize() override;

        void onSync() override;
    };
}

#endif
