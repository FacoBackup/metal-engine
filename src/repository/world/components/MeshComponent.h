#ifndef MESHCOMPONENT_H
#define MESHCOMPONENT_H

#include "../impl/AbstractComponent.h"
#include "../../../util/Serializable.h"
#include <glm/glm.hpp>

namespace Metal {
    struct MeshComponent final : AbstractComponent, Serializable {
        std::string meshId;
        std::string materialId;

        glm::vec3 albedoColor{1, 1, 1};
        bool emissiveSurface = false;
        float roughnessFactor = 1;
        float metallicFactor = 0;
        int parallaxLayers = 16;
        unsigned int renderIndex = 0;
        float parallaxHeightScale = 1;
        bool needsReVoxelization = false;

        void registerFields() override;

        void onUpdate(InspectableMember *member) override;

        ComponentTypes::ComponentType getType() override;

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["entityId"] = entityId;
            j["meshId"] = meshId;
            j["materialId"] = materialId;
            j["parallaxHeightScale"] = parallaxHeightScale;
            j["parallaxLayers"] = parallaxLayers;
            j["emissiveSurface"] = emissiveSurface;
            j["albedoColor"] = {albedoColor.x, albedoColor.y, albedoColor.z};
            j["roughnessFactor"] = roughnessFactor;
            j["metallicFactor"] = metallicFactor;
            return j;
        }

        void fromJson(const nlohmann::json& j) override {
            entityId = j.at("entityId").get<EntityID>();
            meshId = j.at("meshId").get<std::string>();
            materialId = j.at("materialId").get<std::string>();
            parallaxHeightScale = j.at("parallaxHeightScale").get<float>();
            parallaxLayers = j.at("parallaxLayers").get<int>();
            emissiveSurface = j.at("emissiveSurface").get<bool>();
            albedoColor.x = j.at("albedoColor")[0].get<float>();
            albedoColor.y = j.at("albedoColor")[1].get<float>();
            albedoColor.z = j.at("albedoColor")[2].get<float>();
            roughnessFactor = j.at("roughnessFactor").get<float>();
            metallicFactor = j.at("metallicFactor").get<float>();
        }
    };
}
#endif //MESHCOMPONENT_H
