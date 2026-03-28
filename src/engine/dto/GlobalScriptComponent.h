#ifndef GLOBAL_SCRIPT_COMPONENT_H
#define GLOBAL_SCRIPT_COMPONENT_H

#include "AbstractComponent.h"


namespace Metal {
    struct GlobalScriptComponent final : AbstractComponent {
        std::string scriptPath;

    protected:
        void registerFields() override;

    public:
        ComponentType getType() const override;
    };
}
#endif //GLOBAL_SCRIPT_COMPONENT_H
