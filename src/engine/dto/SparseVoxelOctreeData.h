#ifndef SPARSEVOXELOCTREEDATA_H
#define SPARSEVOXELOCTREEDATA_H

#include <glm/vec4.hpp>
#include <vector>

#include "../../common/Reflection.h"

namespace Metal {
    struct SparseVoxelOctreeData final : Reflection {
        std::vector<unsigned int> data{};
        unsigned int voxelBufferOffset;

        void registerFields() override {
            registerSerializableOnlyField(&voxelBufferOffset, INT, "voxelBufferOffset");
            registerSerializableOnlyField(&data, COMPOSITE, "data").setTransformer(
                [this] {
                    nlohmann::json j = nlohmann::json::array();
                    for (unsigned int c: data) {
                        if (c) j.push_back(c);
                    }
                    return j;
                },
                [this](const nlohmann::json &j) {
                    data = j.at("data").get<std::vector<unsigned int> >();
                });
        }
    };
}

#endif //SPARSEVOXELOCTREEDATA_H
