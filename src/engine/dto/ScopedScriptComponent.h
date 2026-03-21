#ifndef SCOPED_SCRIPT_COMPONENT_H
#define SCOPED_SCRIPT_COMPONENT_H

#include "AbstractComponent.h"
#include "../../common/ISerialize.h"

namespace Metal {
    struct ScopedScriptComponent final : AbstractComponent, ISerialize {
        std::string onCreatePath;
        std::string onUpdatePath;
        std::string onDestroyPath;

        void registerFields() override;

        ComponentType getType() override;

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json& j) override;
    };
}
#endif //SCOPED_SCRIPT_COMPONENT_H
