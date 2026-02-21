#ifndef WORLDREPOSITORY_H
#define WORLDREPOSITORY_H

#include <memory>
#include <unordered_map>
#include <vector>
#include <glm/ext/scalar_constants.hpp>

#include "../../common/AbstractRuntimeComponent.h"
#include "../../util/Serializable.h"
#include "../../enum/engine-definitions.h"
#include "../../service/camera/Camera.h"
#include "impl/Entity.h"
#include "../../enum/ComponentType.h"
#include "components/LightComponent.h"
#include "components/MeshComponent.h"
#include "components/SphereLightComponent.h"
#include "components/PlaneLightComponent.h"
#include "components/TransformComponent.h"
#include "components/VolumeComponent.h"

namespace Metal {
    struct WorldRepository final : AbstractRuntimeComponent, Serializable {
        static constexpr EntityID ROOT_ID = 1;

        explicit WorldRepository();

        Camera camera{-(glm::pi<float>() / 4), glm::pi<float>() / 4, {10, 10, 10}};
        std::unordered_map<EntityID, Entity> entities{};
        std::unordered_map<EntityID, MeshComponent> meshes{};
        std::unordered_map<EntityID, TransformComponent> transforms{};
        std::unordered_map<EntityID, std::unique_ptr<LightComponent>> lights{};
        std::unordered_map<EntityID, VolumeComponent> volumes{};
        std::unordered_map<EntityID, bool> culled{};
        std::unordered_map<EntityID, bool> hiddenEntities{};

        void createComponent(EntityID entity, ComponentTypes::ComponentType type);

        EntityID createEntity(std::string name = "New Entity", bool container = false);

        void linkEntities(const Entity *parentEntity, Entity *child);

        [[nodiscard]] Entity *getEntity(EntityID node);

        Inspectable *getComponent(ComponentTypes::ComponentType comp, EntityID entity);

        void deleteRecursively(const std::vector<EntityID> &entities);

        void deleteEntities(const std::vector<EntityID> & entities);

        void changeVisibility(EntityID entity, bool isVisible);

    private:
        void changeVisibilityRecursively(EntityID entity, bool isVisible);

        EntityID lastId = ROOT_ID;

    public:
        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["camera"] = camera.toJson();
            j["lastId"] = lastId;

            nlohmann::json e;
            for (auto const& [key, val] : entities) e[std::to_string(key)] = val.toJson();
            j["entities"] = e;

            nlohmann::json m;
            for (auto const& [key, val] : meshes) m[std::to_string(key)] = val.toJson();
            j["meshes"] = m;

            nlohmann::json t;
            for (auto const& [key, val] : transforms) t[std::to_string(key)] = val.toJson();
            j["transforms"] = t;

            nlohmann::json l;
            for (auto const& [key, val] : lights) l[std::to_string(key)] = val->toJson();
            j["lights"] = l;

            nlohmann::json v;
            for (auto const& [key, val] : volumes) v[std::to_string(key)] = val.toJson();
            j["volumes"] = v;

            j["hiddenEntities"] = hiddenEntities;
            return j;
        }

        void fromJson(const nlohmann::json& j) override {
            if (j.contains("camera")) camera.fromJson(j.at("camera"));
            if (j.contains("lastId")) lastId = j.at("lastId").get<EntityID>();

            entities.clear();
            if (j.contains("entities")) {
                for (auto const& [key, val] : j.at("entities").items()) {
                    Entity entity;
                    entity.fromJson(val);
                    entities.emplace(std::stoull(key), entity);
                }
            }

            meshes.clear();
            if (j.contains("meshes")) {
                for (auto const& [key, val] : j.at("meshes").items()) {
                    MeshComponent mesh;
                    mesh.fromJson(val);
                    meshes.emplace(std::stoull(key), mesh);
                }
            }

            transforms.clear();
            if (j.contains("transforms")) {
                for (auto const& [key, val] : j.at("transforms").items()) {
                    TransformComponent trans;
                    trans.fromJson(val);
                    transforms.emplace(std::stoull(key), trans);
                }
            }

            lights.clear();
            if (j.contains("lights")) {
                for (auto const& [key, val] : j.at("lights").items()) {
                    std::string type = val.at("lightType").get<std::string>();
                    if (type == "SPHERE") {
                        auto light = std::make_unique<SphereLightComponent>();
                        light->fromJson(val);
                        lights.emplace(std::stoull(key), std::move(light));
                    } else if (type == "PLANE") {
                        auto light = std::make_unique<PlaneLightComponent>();
                        light->fromJson(val);
                        lights.emplace(std::stoull(key), std::move(light));
                    }
                }
            }

            volumes.clear();
            if (j.contains("volumes")) {
                for (auto const& [key, val] : j.at("volumes").items()) {
                    VolumeComponent vol;
                    vol.fromJson(val);
                    volumes.emplace(std::stoull(key), vol);
                }
            }

            if (j.contains("hiddenEntities")) {
                hiddenEntities = j.at("hiddenEntities").get<std::unordered_map<EntityID, bool>>();
            }
        }
    };
} // Metal

#endif //WORLDREPOSITORY_H
