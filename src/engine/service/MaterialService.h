#ifndef MATERIALSERVICE_H
#define MATERIALSERVICE_H

namespace Metal {
    struct PrimitiveComponent;
    struct MeshMetadata;

    class MaterialService final {
    public:
        void load(MeshMetadata &data, PrimitiveComponent &component);
    };
} // Metal

#endif //MATERIALSERVICE_H
