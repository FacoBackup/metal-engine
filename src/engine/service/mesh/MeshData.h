#ifndef MESHDATA_H
#define MESHDATA_H
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Metal {
    struct MeshData {
        std::string name;
        std::vector<glm::vec3> vertices;
        std::vector<uint32_t> indices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;
    };
} // Metal

#endif //MESHDATA_H
