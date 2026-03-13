#ifndef MATERIALSERVICE_H
#define MATERIALSERVICE_H

namespace Metal {
    struct PrimitiveComponent;
    struct MeshMetadata;
    class TextureService;

    class MaterialService final {
        TextureService &textureService;
    public:
        explicit MaterialService(TextureService &textureService) : textureService(textureService) {}
        MaterialService() = delete;

        void load(MeshMetadata &data, PrimitiveComponent &component);
    };
} // Metal

#endif //MATERIALSERVICE_H
