#ifndef SCOPED_SCRIPT_COMPONENT_H
#define SCOPED_SCRIPT_COMPONENT_H

#include "AbstractComponent.h"


namespace Metal {
    struct ScopedScriptComponent final : AbstractComponent {
        std::string onCreatePath;
        std::string onUpdatePath;
        std::string onDestroyPath;

    protected:
        void registerFields() override;

    public:
        ComponentType getType() const override;
    };
}
#endif //SCOPED_SCRIPT_COMPONENT_H
