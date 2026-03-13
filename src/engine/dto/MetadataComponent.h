#ifndef ENTITY_H
#define ENTITY_H
#include <unordered_map>
#include <glm/vec3.hpp>

#include "AbstractComponent.h"
#include "../../editor/enum/engine-definitions.h"
#include "../enum/ComponentType.h"
#include "../../../common/Inspectable.h"
#include "../../../common/Serializable.h"

namespace Metal {
    struct MetadataComponent final : AbstractComponent, Serializable {
        std::string name = "New entity";
        glm::vec3 color{1,1,1};

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["name"] = name;
            j["color"] = {color.x, color.y, color.z};
            return j;
        }

        void fromJson(const nlohmann::json &j) override {
            name = j.at("name").get<std::string>();
            color.x = j.at("color")[0].get<float>();
            color.y = j.at("color")[1].get<float>();
            color.z = j.at("color")[2].get<float>();
        }

        void registerFields() override;

        ComponentType getType() override;
    };
} // Metal

#endif //ENTITY_H
