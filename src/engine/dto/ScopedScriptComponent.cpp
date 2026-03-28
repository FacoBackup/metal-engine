#include "ScopedScriptComponent.h"
#include "../../common/FileExtensions.h"

namespace Metal {
    void ScopedScriptComponent::registerFields() {
        registerEditableField<RESOURCE>(&onCreatePath).setName("On Create").setGroup("").setSupportedFileTypes(Metal::FileExtensions::scripts);
        registerEditableField<RESOURCE>(&onUpdatePath).setName("On Update").setGroup("").setSupportedFileTypes(Metal::FileExtensions::scripts);
        registerEditableField<RESOURCE>(&onDestroyPath).setName("On Destroy").setGroup("").setSupportedFileTypes(Metal::FileExtensions::scripts);
    }

    ComponentType ScopedScriptComponent::getType() const {
        return SCOPED_SCRIPT;
    }
}
