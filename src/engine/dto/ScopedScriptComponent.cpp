#include "ScopedScriptComponent.h"
#include "../../common/FileExtensions.h"

namespace Metal {
    void ScopedScriptComponent::registerFields() {
        registerResourceSelection(onCreatePath, "", "On Create", SCRIPT_EXTENSIONS);
        registerResourceSelection(onUpdatePath, "", "On Update", SCRIPT_EXTENSIONS);
        registerResourceSelection(onDestroyPath, "", "On Destroy", SCRIPT_EXTENSIONS);
    }

    ComponentType ScopedScriptComponent::getType() {
        return ComponentType::SCOPED_SCRIPT;
    }

    nlohmann::json ScopedScriptComponent::toJson() const {
        nlohmann::json j;
        j["entityId"] = entityId;
        j["onCreatePath"] = onCreatePath;
        j["onUpdatePath"] = onUpdatePath;
        j["onDestroyPath"] = onDestroyPath;
        return j;
    }

    void ScopedScriptComponent::fromJson(const nlohmann::json &j) {
        entityId = j.at("entityId").get<entt::entity>();
        onCreatePath = j.value("onCreatePath", "");
        onUpdatePath = j.value("onUpdatePath", "");
        onDestroyPath = j.value("onDestroyPath", "");
    }
}
