#ifndef MESHDATA_H
#define MESHDATA_H
#include <vector>
#include <glm/vec3.hpp>

namespace Metal {
    struct MeshData {
        std::vector<glm::vec3> vertices;
        std::vector<uint32_t> indices;
        std::vector<glm::vec3> normals;
    };
} // Metal

#endif //MESHDATA_H
