#ifndef INSPECTIONTOOLPROVIDER_H
#define INSPECTIONTOOLPROVIDER_H

#include "../dto/IToolProvider.h"

namespace Metal {
    struct WorldRepository;
    struct FieldMetadata;

    class InspectionToolProvider final : public IToolProvider {
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
        std::string listRepositories() const;

        std::string getComponentInfo(const nlohmann::json &params) const;

        std::string changeComponentField(const nlohmann::json &params) const;

        std::string changeRepositoryField(const nlohmann::json &params) const;

        static std::string updateField(FieldMetadata *member, const nlohmann::json &value);
    };
}

#endif
