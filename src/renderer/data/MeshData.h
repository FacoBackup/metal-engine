#ifndef MESHDATA_H
#define MESHDATA_H
#include <vector>
#include "VertexData.h"
#include "../../common/serialization-definitions.h"

namespace Metal {
    struct LevelOfDetail;

    struct MeshData final {
        std::string name;
        std::vector<VertexData> data;
        std::vector<unsigned int > indices;

        explicit MeshData(std::string name = "", const std::vector<VertexData> &data = {},
                          const std::vector<unsigned int > &indices = {})
            : name(std::move(name)),
              data(data),
              indices(indices) {
        }

        SAVE_TEMPLATE(name, indices, data)
    };
} // Metal

#endif //MESHDATA_H
