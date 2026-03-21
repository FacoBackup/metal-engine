#include "GlobalScriptComponent.h"
#include "../../common/FileExtensions.h"

namespace Metal {
    void GlobalScriptComponent::registerFields() {
        registerResourceSelection(scriptPath, "", "Script Path", SCRIPT_EXTENSIONS);
    }

    ComponentType GlobalScriptComponent::getType() {
        return ComponentType::GLOBAL_SCRIPT;
    }

    nlohmann::json GlobalScriptComponent::toJson() const {
        nlohmann::json j;
        j["entityId"] = entityId;
        j["scriptPath"] = scriptPath;
        return j;
    }

    void GlobalScriptComponent::fromJson(const nlohmann::json &j) {
        entityId = j.at("entityId").get<entt::entity>();
        scriptPath = j.value("scriptPath", "");
    }
}
