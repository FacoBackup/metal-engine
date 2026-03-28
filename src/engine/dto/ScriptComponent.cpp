#include "ScriptComponent.h"

namespace Metal {
    void ScriptComponent::registerFields() {
        AbstractComponent::registerFields();
        registerEditableField<STRING>(&scriptPath).setName("scriptPath").setLabel("Script path");
        registerEditableField<BOOLEAN>(&runAsync).setName("runAsync").setLabel("Run asynchronously");
    }

    ComponentType ScriptComponent::getType() const {
        return SCRIPT;
    }
}
