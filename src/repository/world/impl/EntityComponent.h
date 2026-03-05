#ifndef ENTITY_H
#define ENTITY_H
#include <unordered_map>
#include <glm/vec3.hpp>

#include "../../../enum/engine-definitions.h"
#include "../../../enum/ComponentType.h"
#include "../../../common/inspection/Inspectable.h"
#include "../../../util/Serializable.h"

namespace Metal {
    struct EntityComponent final : Inspectable, Serializable {
        std::string name = "New entity";
        glm::vec3 color{};
        bool isContainer = false;

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["name"] = name;
            j["color"] = {color.x, color.y, color.z};
            j["isContainer"] = isContainer;
            return j;
        }

        void fromJson(const nlohmann::json &j) override {
            name = j.at("name").get<std::string>();
            color.x = j.at("color")[0].get<float>();
            color.y = j.at("color")[1].get<float>();
            color.z = j.at("color")[2].get<float>();
            isContainer = j.at("isContainer").get<bool>();
        }

        EntityComponent() = default;

        void initialize(bool container);

        void registerFields() override;

        const char *getIcon() override;

        const char *getTitle() override {
            return name.c_str();
        }
    };

    struct HierarchyComponent {
        EntityID parent = EMPTY_ENTITY;
        std::vector<EntityID> children{};
    };
} // Metal

#endif //ENTITY_H
