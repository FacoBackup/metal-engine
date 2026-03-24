#ifndef WORLDTOOLS_H
#define WORLDTOOLS_H

#include "../dto/IToolProvider.h"

namespace Metal {
    struct WorldRepository;

    class WorldToolProvider final : public IToolProvider {
        WorldRepository *worldRepository = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"WorldRepository", &worldRepository}
            };
        }

    protected:
        void registerTools() override;

    private:
        std::string createEntity() const;

        std::string deleteEntity(const nlohmann::json &params);

        static std::string listComponentTypes();

        std::string addComponent(const nlohmann::json &params) const;

        std::string getEntityInfo(const nlohmann::json &params) const;

        std::string listEntities() const;
    };
}

#endif
