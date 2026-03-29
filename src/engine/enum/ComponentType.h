#ifndef COMPONENTTYPE_H
#define COMPONENTTYPE_H
#include <vector>
#include <string>
#include <functional>
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

#define COMPONENTS_LIST \
    X(STATIC_GEOMETRY, "Static geometry", "staticGeo", Icons::view_in_ar, {TRANSFORM}, StaticGeometryComponent) \
    X(INSTANCED_GEOMETRY, "Instanced geometry", "instancedGeo", Icons::layers, {TRANSFORM}, InstancedGeometryComponent) \
    X(ANIMATED_GEOMETRY, "Animated geometry", "animatedGeo", Icons::animation, {TRANSFORM}, AnimatedGeometryComponent) \
    X(TRANSFORM, "Transformation", "transform", Icons::transform, {}, TransformComponent) \
    X(LIGHT, "Light Component", "light", Icons::lightbulb, std::vector({TRANSFORM, STATIC_GEOMETRY}), LightComponent) \
    X(METADATA, "Metadata", "metadata", Icons::data_array, {}, MetadataComponent) \
    X(SCRIPT, "Script Component", "script", Icons::description, {}, ScriptComponent) \
    X(RIGID_BODY, "Rigid Body", "rigid_body", Icons::settings_input_component, {TRANSFORM}, RigidBodyComponent) \
    X(BOX_COLLIDER, "Box Collider", "box_collider", Icons::check_box_outline_blank, {TRANSFORM}, BoxColliderComponent) \
    X(SPHERE_COLLIDER, "Sphere Collider", "sphere_collider", Icons::panorama_fish_eye, {TRANSFORM}, SphereColliderComponent) \
    X(CAPSULE_COLLIDER, "Capsule Collider", "capsule_collider", Icons::reorder, {TRANSFORM}, CapsuleColliderComponent) \
    X(MESH_COLLIDER, "Mesh Collider", "mesh_collider", Icons::filter_none, {TRANSFORM}, MeshColliderComponent) \
    X(CAMERA, "Camera", "camera", Icons::camera_alt, {TRANSFORM}, CameraComponent)

namespace Metal {
    class ApplicationContext;
    class Reflection;
    struct WorldRepository;

    enum ComponentType {
#define X(TYPE, NAME, JSON_KEY, ICON, DEPS, CLASS) TYPE,
        COMPONENTS_LIST
#undef X
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
        std::function<Reflection*(WorldRepository &, entt::entity)> getInspectable;
        std::function<bool(const WorldRepository &, entt::entity)> hasComponent;
    };

    const std::vector<ComponentDefinition> &getComponents();

    const char *NameOf(ComponentType mode);

    const char *IconOf(ComponentType mode);
}
#endif //COMPONENTTYPE_H
