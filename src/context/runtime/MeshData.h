#ifndef MESHDATA_H
#define MESHDATA_H
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "../../common/interface/BinarySerializable.h"

namespace Metal {
    struct LevelOfDetail;

    struct MeshData final : BinarySerializable {
        std::string name;
        std::vector<glm::vec3> vertices;
        std::vector<uint32_t> indices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;

        ~MeshData() override = default;

        void readFields(std::ifstream &file) override {
            // Read `name`
            uint64_t nameLength;
            file.read(reinterpret_cast<char *>(&nameLength), sizeof(nameLength));
            name.resize(nameLength);
            file.read(name.data(), nameLength);

            // Read `vertices`
            uint64_t vertexCount;
            file.read(reinterpret_cast<char *>(&vertexCount), sizeof(vertexCount));
            vertices.resize(vertexCount);
            file.read(reinterpret_cast<char *>(vertices.data()), vertexCount * sizeof(glm::vec3));

            // Read `indices`
            uint64_t indexCount;
            file.read(reinterpret_cast<char *>(&indexCount), sizeof(indexCount));
            indices.resize(indexCount);
            file.read(reinterpret_cast<char *>(indices.data()), indexCount * sizeof(uint32_t));

            // Read `normals`
            uint64_t normalCount;
            file.read(reinterpret_cast<char *>(&normalCount), sizeof(normalCount));
            normals.resize(normalCount);
            file.read(reinterpret_cast<char *>(normals.data()), normalCount * sizeof(glm::vec3));

            // Read `uvs`
            uint64_t uvCount;
            file.read(reinterpret_cast<char *>(&uvCount), sizeof(uvCount));
            uvs.resize(uvCount);
            file.read(reinterpret_cast<char *>(uvs.data()), uvCount * sizeof(glm::vec2));

            file.close();
        }

        void writeFields(std::ofstream &file) override {
            // Write `name` length and data
            uint64_t nameLength = name.size();
            file.write(reinterpret_cast<const char *>(&nameLength), sizeof(nameLength));
            file.write(name.data(), nameLength);

            // Write `vertices`
            uint64_t vertexCount = vertices.size();
            file.write(reinterpret_cast<const char *>(&vertexCount), sizeof(vertexCount));
            file.write(reinterpret_cast<const char *>(vertices.data()), vertexCount * sizeof(glm::vec3));

            // Write `indices`
            uint64_t indexCount = indices.size();
            file.write(reinterpret_cast<const char *>(&indexCount), sizeof(indexCount));
            file.write(reinterpret_cast<const char *>(indices.data()), indexCount * sizeof(uint32_t));

            // Write `normals`
            uint64_t normalCount = normals.size();
            file.write(reinterpret_cast<const char *>(&normalCount), sizeof(normalCount));
            file.write(reinterpret_cast<const char *>(normals.data()), normalCount * sizeof(glm::vec3));

            // Write `uvs`
            uint64_t uvCount = uvs.size();
            file.write(reinterpret_cast<const char *>(&uvCount), sizeof(uvCount));
            file.write(reinterpret_cast<const char *>(uvs.data()), uvCount * sizeof(glm::vec2));
        }
    };
} // Metal

#endif //MESHDATA_H
