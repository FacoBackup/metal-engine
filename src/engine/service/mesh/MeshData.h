#ifndef MESHDATA_H
#define MESHDATA_H
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "../../../common/interface/Serializable.h"

namespace Metal {
    struct LevelOfDetail;

    struct MeshData final : Serializable {
        std::string name;
        std::vector<glm::vec3> vertices;
        std::vector<uint32_t> indices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;

        void readFields(const std::ifstream &ifs) override {
        }

        void writeFields(const std::ofstream &ofs) override {
        }
    };
} // Metal

#endif //MESHDATA_H
