#ifndef SPARSEVOXELOCTREEDATA_H
#define SPARSEVOXELOCTREEDATA_H

#include <glm/vec4.hpp>
#include <vector>

#include "../../../util/Serializable.h"

namespace Metal {
    struct SparseVoxelOctreeData final : Serializable {
        std::vector<unsigned int> data{};
        unsigned int voxelBufferOffset;

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["data"] = data;
            j["voxelBufferOffset"] = voxelBufferOffset;
            return j;
        }

        void fromJson(const nlohmann::json& j) override {
            data = j.at("data").get<std::vector<unsigned int>>();
            voxelBufferOffset = j.at("voxelBufferOffset").get<unsigned int>();
        }
    };
}

#endif //SPARSEVOXELOCTREEDATA_H
