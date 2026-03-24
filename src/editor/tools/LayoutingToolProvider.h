#ifndef LAYOUTINGTOOLPROVIDER_H
#define LAYOUTINGTOOLPROVIDER_H

#include <glm/vec3.hpp>

#include "../dto/IToolProvider.h"

namespace Metal {
    class WorldRepository;
    class FilesService;

    class LayoutingToolProvider final : public IToolProvider {
        WorldRepository *worldRepository = nullptr;
        FilesService *filesService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"WorldRepository", &worldRepository},
                {"FilesService", &filesService}
            };
        }

    protected:
        void registerTools() override;

    private:
        std::string listMeshFiles(const nlohmann::json &params);
        std::string createMeshEntity(const nlohmann::json &params);
        std::string setEntityTransform(const nlohmann::json &params);

        static glm::vec3 parseVec3(const std::string &s);
    };
}

#endif // LAYOUTINGTOOLPROVIDER_H
