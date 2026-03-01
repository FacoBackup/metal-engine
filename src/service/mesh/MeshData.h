#ifndef MESHDATA_H
#define MESHDATA_H
#include <string>
#include <vector>
#include "VertexData.h"
#include "../../util/serialization-definitions.h"

namespace Metal {
    struct MeshData final {
        std::string name;
        std::vector<VertexData> data;
        std::vector<unsigned int> indices;
        glm::vec3 gizmoCenter{};

        explicit MeshData(std::string name = "", const std::vector<VertexData> &data = {},
                          const std::vector<unsigned int> &indices = {},
                          const glm::vec3 &gizmoCenter = {})
            : name(std::move(name)),
              data(data),
              indices(indices),
              gizmoCenter(gizmoCenter) {
        }

        SAVE_TEMPLATE(name, indices, data, gizmoCenter.x, gizmoCenter.y, gizmoCenter.z)
    };
} // Metal

#endif //MESHDATA_H
