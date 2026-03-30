#ifndef METAL_ENGINE_LUASERVICE_H
#define METAL_ENGINE_LUASERVICE_H

#include "common/IService.h"
#include "common/IInit.h"
#include "common/IDisposable.h"
#include "common/IEventMember.h"
#include "common/ISync.h"
#include "../util/LuaContext.h"
#include "common/IAsyncSync.h"
#include <string>
#include <mutex>
#include <entt/entity/entity.hpp>

namespace Metal {
    struct ScriptComponent;
    struct WorldRepository;

    class EngineContext;
    class TransformService;

    class LuaService final : public IService, public IInit, public IEventMember, public ISync, public IAsyncSync {
        LuaContext luaContext;
        bool enabled = false;
        bool playStarted = false;
        WorldRepository *worldRepository = nullptr;
        EngineContext *engineContext = nullptr;
        TransformService *transformService = nullptr;
        std::unordered_map<std::string, sol::protected_function> scriptCache;
        std::vector<entt::entity> activeUpdateScripts;
        std::vector<entt::entity> activeAsyncUpdateScripts;
        std::mutex luaMutex;

    public:
        LuaService() = default;

        ~LuaService() override = default;

        std::vector<Dependency> getDependencies() override {
            return {
                {"WorldRepository", &worldRepository},
                {"EngineContext", &engineContext},
                {"TransformService", &transformService}
            };
        }

        void onInitialize() override;

        void onScriptComponentAdded(entt::registry &registry, entt::entity entity);

        void onScriptComponentUpdated(entt::registry &registry, entt::entity entity);

        void onScriptComponentRemoved(entt::registry &registry, entt::entity entity);

        void onSync() override;

        // IAsyncSync implementation
        [[nodiscard]] std::string getSyncThreadId() const override { return "lua_async_scripting"; }

        float getSyncInterval() const override { return 16.0f; } // ~60fps

        void onAsyncSync() override;

        /**
         * Executes a lua script string.
         * @param script The lua script code.
         * @return true if execution was successful.
         */
        bool executeString(const std::string &script);

        /**
         * Executes a lua script file.
         * @param path The path to the lua file.
         * @return true if execution was successful.
         */
        bool executeFile(const std::string &path);

        /**
         * Access the lua state directly for bindings.
         */
        sol::state &getState() { return luaContext.getState(); }

    private:
        void loadScript(ScriptComponent &comp, entt::entity entity);
    };
} // Metal

#endif //METAL_ENGINE_LUASERVICE_H
