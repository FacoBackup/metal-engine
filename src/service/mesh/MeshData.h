#ifndef MESHDATA_H
#define MESHDATA_H
#include <vector>
#include "VertexData.h"
#include "../../util/Serializable.h"

namespace Metal {
    struct LevelOfDetail;

    struct MeshData final : Serializable {
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

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["name"] = name;
            j["indices"] = indices;
            j["gizmoCenter"] = {gizmoCenter.x, gizmoCenter.y, gizmoCenter.z};
            nlohmann::json d = nlohmann::json::array();
            for (const auto& v : data) {
                d.push_back(v.toJson());
            }
            j["data"] = d;
            return j;
        }

        void fromJson(const nlohmann::json& j) override {
            name = j.at("name").get<std::string>();
            indices = j.at("indices").get<std::vector<unsigned int>>();
            gizmoCenter = {j.at("gizmoCenter")[0], j.at("gizmoCenter")[1], j.at("gizmoCenter")[2]};
            data.clear();
            for (const auto& v : j.at("data")) {
                VertexData vd;
                vd.fromJson(v);
                data.push_back(vd);
            }
        }
    };
} // Metal

#endif //MESHDATA_H
