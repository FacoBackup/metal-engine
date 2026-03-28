#include "ScopedScriptComponent.h"
#include "../../common/FileExtensions.h"

namespace Metal {
    void ScopedScriptComponent::registerFields() {
        registerEditableField(&onCreatePath, RESOURCE, "On Create", "").setSupportedFileTypes(Metal::FileExtensions::scripts);
        registerEditableField(&onUpdatePath, RESOURCE, "On Update", "").setSupportedFileTypes(Metal::FileExtensions::scripts);
        registerEditableField(&onDestroyPath, RESOURCE, "On Destroy", "").setSupportedFileTypes(Metal::FileExtensions::scripts);
    }

    ComponentType ScopedScriptComponent::getType() const {
        return SCOPED_SCRIPT;
    }
}
