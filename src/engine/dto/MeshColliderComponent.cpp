#include "MeshColliderComponent.h"
#include "../../ApplicationContext.h"
#include "../../common/FileExtensions.h"

namespace Metal {
    void MeshColliderComponent::registerFields() {
        static const std::vector<const FileExtensionInfo*> meshExtensions = {Metal::FileExtensions::geometry.get()};
        registerEditableField<RESOURCE>(&meshId).setName("meshId").setLabel("Mesh").setSupportedFileTypes(meshExtensions);
        registerSerializableOnlyField<BOOLEAN>(&isConvex).setName("isConvex");
    }

    ComponentType MeshColliderComponent::getType() const {
        return MESH_COLLIDER;
    }
}
