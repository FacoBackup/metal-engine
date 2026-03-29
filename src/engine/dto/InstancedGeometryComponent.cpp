#include "InstancedGeometryComponent.h"
#include "../../ApplicationContext.h"
#include "../../common/FileExtensions.h"

namespace Metal {
    void InstancedGeometryComponent::registerFields() {
        static const std::vector<const FileExtensionInfo *> meshExtensions = {Metal::FileExtensions::mesh.get()};
        static const std::vector<const FileExtensionInfo *> textureExtensions = {
            Metal::FileExtensions::png.get(), Metal::FileExtensions::jpg.get(),
            Metal::FileExtensions::jpeg.get(),
            Metal::FileExtensions::tga.get()
        };
        registerEditableField<RESOURCE>(&meshId).setName("meshId").setLabel("Mesh").setSupportedFileTypes(meshExtensions);
        registerEditableField<RESOURCE>(&albedo).setName("albedo").setLabel("Albedo").setSupportedFileTypes(textureExtensions);
        registerEditableField<RESOURCE>(&roughness).setName("roughness").setLabel("Roughness").setSupportedFileTypes(textureExtensions);
        registerEditableField<RESOURCE>(&metallic).setName("metallic").setLabel("Metallic").setSupportedFileTypes(textureExtensions);
        registerEditableField<RESOURCE>(&normal).setName("normal").setLabel("Normal").setSupportedFileTypes(textureExtensions);
        registerEditableField<RESOURCE>(&height).setName("height").setLabel("Height").setSupportedFileTypes(textureExtensions);
        registerEditableField<FLOAT>(&parallaxScale).setName("parallaxScale").setLabel("Parallax Scale").setMin(0).setMax(1);
        registerSerializableOnlyField<UINT>(&renderIndex).setName("renderIndex");
    }

    ComponentType InstancedGeometryComponent::getType() const {
        return INSTANCED_GEOMETRY;
    }
}
