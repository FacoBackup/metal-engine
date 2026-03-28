#include "MeshColliderComponent.h"
#include "../../ApplicationContext.h"
#include "../../common/FileExtensions.h"

namespace Metal {
    void MeshColliderComponent::registerFields() {
        static const std::vector<const FileExtensionInfo*> meshExtensions = {Metal::FileExtensions::mesh.get()};
        registerEditableField(&meshId, RESOURCE, "Mesh", "").setSupportedFileTypes(meshExtensions);
        registerSerializableOnlyField(&isConvex, BOOLEAN, "isConvex");
    }

    ComponentType MeshColliderComponent::getType() const {
        return MESH_COLLIDER;
    }
}
