#ifndef GLOBAL_SCRIPT_COMPONENT_H
#define GLOBAL_SCRIPT_COMPONENT_H

#include "AbstractComponent.h"
#include "../../common/ISerialize.h"

namespace Metal {
    struct GlobalScriptComponent final : AbstractComponent, ISerialize {
        std::string scriptPath;

        void registerFields() override;

        ComponentType getType() override;

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json& j) override;
    };
}
#endif //GLOBAL_SCRIPT_COMPONENT_H
