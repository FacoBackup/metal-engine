#include "PrimitiveComponent.h"
#include "../../ApplicationContext.h"
#include "../../common/FileExtensions.h"
#include "MeshData.h"

namespace Metal {
    void PrimitiveComponent::registerFields() {
        static const std::vector<const FileExtensionInfo *> meshExtensions = {Metal::FileExtensions::mesh.get()};
        static const std::vector<const FileExtensionInfo *> textureExtensions = {
            Metal::FileExtensions::png.get(), Metal::FileExtensions::jpg.get(),
            Metal::FileExtensions::jpeg.get(),
            Metal::FileExtensions::tga.get()
        };
        registerEditableField<RESOURCE>(&meshId).setName("meshId").setLabel("Mesh").setSupportedFileTypes(meshExtensions);
        registerEditableField<FLOAT>(&transmissionFactor).setName("transmissionFactor").setLabel("Transmission factor").setMin(0).setMax(1);
        registerEditableField<FLOAT>(&thicknessFactor).setName("thicknessFactor").setLabel("Thickness factor").setMin(0).setMax(10);
        registerEditableField<FLOAT>(&ior).setName("ior").setLabel("IOR").setMin(1).setMax(3);
        registerEditableField<RESOURCE>(&albedo).setName("albedo").setLabel("Albedo").setSupportedFileTypes(textureExtensions);
        registerEditableField<RESOURCE>(&roughness).setName("roughness").setLabel("Roughness").setSupportedFileTypes(textureExtensions);
        registerEditableField<RESOURCE>(&metallic).setName("metallic").setLabel("Metallic").setSupportedFileTypes(textureExtensions);
        registerSerializableOnlyField<UINT>(&renderIndex).setName("renderIndex");
    }

    ComponentType PrimitiveComponent::getType() const {
        return PRIMITIVE;
    }
}
