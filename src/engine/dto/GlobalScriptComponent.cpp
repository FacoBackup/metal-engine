#include "GlobalScriptComponent.h"
#include "../../common/FileExtensions.h"

namespace Metal {
    void GlobalScriptComponent::registerFields() {
        registerEditableField(&scriptPath, RESOURCE, "Script Path", "").setSupportedFileTypes(Metal::FileExtensions::scripts);
    }

    ComponentType GlobalScriptComponent::getType() const {
        return GLOBAL_SCRIPT;
    }
}
