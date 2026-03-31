#ifndef SCRIPT_COMPONENT_H
#define SCRIPT_COMPONENT_H

#include "AbstractComponent.h"
#include <string>
#include <sol/sol.hpp>

namespace Metal {
    struct ScriptComponent final : AbstractComponent {
        std::string scriptPath;
        sol::table instance;
        sol::environment env;
        sol::protected_function updateFunc;
        bool runAsync = false;
        bool isErrored = false;

    protected:
        void registerFields() override;

    public:
        ComponentType getType() const override;
    };
}

#endif //SCRIPT_COMPONENT_H
