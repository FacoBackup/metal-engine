#ifndef MESHDATA_H
#define MESHDATA_H
#include <vector>
#include "VertexData.h"
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

namespace Metal {
    struct LevelOfDetail;

    struct MeshData final {
        std::string name;
        std::vector<VertexData> data;
        std::vector<uint32_t> indices;

        explicit MeshData(std::string name = "", const std::vector<VertexData> &data = {},
                          const std::vector<uint32_t> &indices = {})
            : name(std::move(name)),
              data(data),
              indices(indices) {
        }

        template<class Archive>
        void save(Archive &ar) const {
            ar(name, indices, data);
        }

        template<class Archive>
        void load(Archive &ar) {
            ar(name, indices, data);
        }
    };
} // Metal

#endif //MESHDATA_H
