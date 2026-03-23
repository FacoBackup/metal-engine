#ifndef ENTITY_H
#define ENTITY_H
#include <unordered_map>
#include <glm/vec3.hpp>

#include "AbstractComponent.h"
#include "editor/enum/engine-definitions.h"
#include "../enum/ComponentType.h"
#include "../../common/Inspectable.h"
#include "../../common/ISerialize.h"

namespace Metal {
    struct MetadataComponent final : AbstractComponent, ISerialize {
        std::string name = "New entity";

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["name"] = name;
            return j;
        }

        void fromJson(const nlohmann::json &j) override {
            name = j.at("name").get<std::string>();
        }

        void registerFields() override;

        ComponentType getType() const override;
    };
} // Metal

#endif //ENTITY_H
