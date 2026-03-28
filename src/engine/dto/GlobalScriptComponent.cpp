#include "GlobalScriptComponent.h"
#include "../../common/FileExtensions.h"

namespace Metal {
    void GlobalScriptComponent::registerFields() {
        registerEditableField<RESOURCE>(&scriptPath).setName("Script Path").setGroup("").setSupportedFileTypes(Metal::FileExtensions::scripts);
    }

    ComponentType GlobalScriptComponent::getType() const {
        return GLOBAL_SCRIPT;
    }
}
