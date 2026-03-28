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
            registerSerializableOnlyField<UINT>(&voxelBufferOffset).setName("voxelBufferOffset");

            auto dataToJson = [this] {
                nlohmann::json j = nlohmann::json::array();
                for (unsigned int c: data) {
                    if (c) j.push_back(c);
                }
                return j;
            };

            auto dataFromJson = [this](const nlohmann::json &j) {
                data = j.get<std::vector<unsigned int>>();
            };

            registerGenericField(dataToJson, dataFromJson).setName("data");
        }
    };
}

#endif //SPARSEVOXELOCTREEDATA_H
