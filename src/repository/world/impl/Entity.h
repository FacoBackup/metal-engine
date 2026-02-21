#ifndef ENTITY_H
#define ENTITY_H
#include <unordered_map>
#include <glm/vec3.hpp>

#include "../../../enum/engine-definitions.h"
#include "../../../enum/ComponentType.h"
#include "../../../common/inspection/Inspectable.h"
#include "../../../util/Serializable.h"

namespace Metal {
    struct AbstractComponent;

    class Entity final : public Inspectable, public Serializable {
        EntityID id;

    public:
        std::string onTile;
        std::string name = "New entity";
        glm::vec3 color{};
        bool isContainer = false;
        std::vector<ComponentTypes::ComponentType> components{};
        std::vector<EntityID> children{};
        EntityID parent = EMPTY_ENTITY;

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["id"] = id;
            j["parent"] = parent;
            j["onTile"] = onTile;
            j["name"] = name;
            j["color"] = {color.x, color.y, color.z};
            j["isContainer"] = isContainer;
            j["components"] = components;
            j["children"] = children;
            return j;
        }

        void fromJson(const nlohmann::json& j) override {
            id = j.at("id").get<EntityID>();
            parent = j.at("parent").get<EntityID>();
            onTile = j.at("onTile").get<std::string>();
            name = j.at("name").get<std::string>();
            color.x = j.at("color")[0].get<float>();
            color.y = j.at("color")[1].get<float>();
            color.z = j.at("color")[2].get<float>();
            isContainer = j.at("isContainer").get<bool>();
            components = j.at("components").get<std::vector<ComponentTypes::ComponentType>>();
            children = j.at("children").get<std::vector<EntityID>>();
        }

        Entity() = default;

        void initialize(EntityID id, bool container);

        [[nodiscard]] EntityID getId() const {
            return id;
        }

        void registerFields() override;

        const char *getIcon() override;

        const char *getTitle() override {
            return name.c_str();
        }
    };
} // Metal

#endif //ENTITY_H
