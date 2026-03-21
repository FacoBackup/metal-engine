#ifndef COMPONENTTYPE_H
#define COMPONENTTYPE_H
#include <vector>
#include <string>
#include <functional>
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

namespace Metal {
    class ApplicationContext;
    class Inspectable;
    struct WorldRepository;

    enum ComponentType {
        PRIMITIVE,
        TRANSFORM,
        VOLUME,
        METADATA,
        LIGHT
    };
}

namespace Metal::ComponentTypes {
    struct ComponentDefinition {
        ComponentType type;
        std::string name;
        std::string jsonKey;
        std::string icon;
        std::vector<ComponentType> dependencies;
        std::function<void(WorldRepository &, entt::entity)> creator;
        std::function<nlohmann::json(WorldRepository &, entt::entity)> toJson;
        std::function<void(WorldRepository &, entt::entity, const nlohmann::json &)> fromJson;
        std::function<Inspectable*(WorldRepository &, entt::entity)> getInspectable;
        std::function<bool(const WorldRepository &, entt::entity)> hasComponent;
    };

    const std::vector<ComponentDefinition> &getComponents();

    const char *NameOf(ComponentType mode);

    const char *IconOf(ComponentType mode);
}
#endif //COMPONENTTYPE_H
