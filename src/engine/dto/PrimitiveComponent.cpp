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
        registerEditableField<RESOURCE>(&meshId).setName("Mesh").setGroup("").setSupportedFileTypes(meshExtensions);
        registerEditableField<FLOAT>(&transmissionFactor).setName("Transmission factor").setGroup("").setMin(0).setMax(1);
        registerEditableField<FLOAT>(&thicknessFactor).setName("Thickness factor").setGroup("").setMin(0).setMax(10);
        registerEditableField<FLOAT>(&ior).setName("IOR").setGroup("").setMin(1).setMax(3);
        registerEditableField<RESOURCE>(&albedo).setName("Albedo").setGroup("").setSupportedFileTypes(textureExtensions);
        registerEditableField<RESOURCE>(&roughness).setName("Roughness texture").setGroup("").setSupportedFileTypes(textureExtensions);
        registerEditableField<RESOURCE>(&metallic).setName("Metallic Texture").setGroup("").setSupportedFileTypes(textureExtensions);
        registerSerializableOnlyField<UINT>(&renderIndex).setName("renderIndex");
    }

    ComponentType PrimitiveComponent::getType() const {
        return PRIMITIVE;
    }
}
