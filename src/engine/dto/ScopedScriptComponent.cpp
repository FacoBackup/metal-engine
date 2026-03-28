#include "ScopedScriptComponent.h"
#include "../../common/FileExtensions.h"

namespace Metal {
    void ScopedScriptComponent::registerFields() {
        registerResourceSelection(onCreatePath, "", "On Create", Metal::FileExtensions::scripts);
        registerResourceSelection(onUpdatePath, "", "On Update", Metal::FileExtensions::scripts);
        registerResourceSelection(onDestroyPath, "", "On Destroy", Metal::FileExtensions::scripts);
    }

    ComponentType ScopedScriptComponent::getType() const {
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
