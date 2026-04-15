#include "InstancedGeometryComponent.h"
#include "../../ApplicationContext.h"
#include "../../common/FileExtensions.h"

namespace Metal {
    void InstancedGeometryComponent::registerFields() {
        static const std::vector<const FileExtensionInfo *> meshExtensions = {Metal::FileExtensions::geometry.get()};
        static const std::vector<const FileExtensionInfo *> materialExtensions = {Metal::FileExtensions::material.get()};
        registerEditableField<RESOURCE>(&meshId).setName("meshId").setLabel("Mesh").setSupportedFileTypes(meshExtensions);
        registerEditableField<RESOURCE>(&materialId).setName("materialId").setLabel("Material").setSupportedFileTypes(materialExtensions);
        registerSerializableOnlyField<UINT>(&renderIndex).setName("renderIndex");
    }

    ComponentType InstancedGeometryComponent::getType() const {
        return INSTANCED_GEOMETRY;
    }
}
