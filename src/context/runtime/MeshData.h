#ifndef MESHDATA_H
#define MESHDATA_H
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "VertexData.h"
#include "../../common/interface/BinarySerializable.h"

namespace Metal {

    struct LevelOfDetail;

    struct MeshData final : BinarySerializable {
        std::string name;
        std::vector<VertexData> data;
        std::vector<uint32_t> indices;

        MeshData(const std::string &name, const std::vector<VertexData> &data, const std::vector<uint32_t> &indices)
            : name(name),
              data(data),
              indices(indices) {
        }

        ~MeshData() override = default;

        void readFields(std::ifstream &file) override {
            // Read `name`
            uint64_t nameLength;
            file.read(reinterpret_cast<char *>(&nameLength), sizeof(nameLength));
            name.resize(nameLength);
            file.read(name.data(), nameLength);

            uint64_t vertexCount;
            file.read(reinterpret_cast<char *>(&vertexCount), sizeof(vertexCount));
            data.resize(vertexCount);
            file.read(reinterpret_cast<char *>(data.data()), vertexCount * sizeof(glm::vec3));

            // Read `indices`
            uint64_t indexCount;
            file.read(reinterpret_cast<char *>(&indexCount), sizeof(indexCount));
            indices.resize(indexCount);
            file.read(reinterpret_cast<char *>(indices.data()), indexCount * sizeof(uint32_t));

            file.close();
        }

        void writeFields(std::ofstream &file) override {
            // Write `name` length and data
            uint64_t nameLength = name.size();
            file.write(reinterpret_cast<const char *>(&nameLength), sizeof(nameLength));
            file.write(name.data(), nameLength);

            // Write `vertices`
            uint64_t vertexCount = data.size();
            file.write(reinterpret_cast<const char *>(&vertexCount), sizeof(vertexCount));
            file.write(reinterpret_cast<const char *>(data.data()), vertexCount * sizeof(VertexData));

            // Write `indices`
            uint64_t indexCount = indices.size();
            file.write(reinterpret_cast<const char *>(&indexCount), sizeof(indexCount));
            file.write(reinterpret_cast<const char *>(indices.data()), indexCount * sizeof(uint32_t));
        }
    };
} // Metal

#endif //MESHDATA_H
