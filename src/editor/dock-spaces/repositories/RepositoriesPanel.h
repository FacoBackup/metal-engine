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
        FormPanel *formPanel = nullptr;

        EditorRepository *editorRepository = nullptr;
        EngineRepository *engineRepository = nullptr;
        WorldRepository *worldRepository = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"EditorRepository", editorRepository},
                {"EngineRepository", engineRepository},
                {"WorldRepository", worldRepository}
            };
        }

        void onInitialize() override;
        void onSync() override;
    };
}

#endif
