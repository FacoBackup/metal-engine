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
        registerEditableField(&meshId, RESOURCE, "Mesh", "").setSupportedFileTypes(meshExtensions);
        registerEditableField(&transmissionFactor, FLOAT, "Transmission factor", "").setMin(0).setMax(1);
        registerEditableField(&thicknessFactor, FLOAT, "Thickness factor", "").setMin(0).setMax(10);
        registerEditableField(&ior, FLOAT, "IOR", "").setMin(1).setMax(3);
        registerEditableField(&albedo, RESOURCE, "Albedo", "").setSupportedFileTypes(textureExtensions);
        registerEditableField(&roughness, RESOURCE, "Roughness texture", "").setSupportedFileTypes(textureExtensions);
        registerEditableField(&metallic, RESOURCE, "Metallic Texture", "").setSupportedFileTypes(textureExtensions);
        registerSerializableOnlyField(&renderIndex, INT, "renderIndex");
    }

    ComponentType PrimitiveComponent::getType() const {
        return PRIMITIVE;
    }
}
