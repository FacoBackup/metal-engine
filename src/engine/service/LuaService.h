#ifndef METAL_ENGINE_LUASERVICE_H
#define METAL_ENGINE_LUASERVICE_H

#include "common/IService.h"
#include "common/IInit.h"
#include "common/IDisposable.h"
#include "common/IEventMember.h"
#include "common/ISync.h"
#include "../util/LuaContext.h"
#include <string>

namespace Metal {
    struct WorldRepository;

    class LuaService final : public IService, public IInit, public IDisposable, public IEventMember, public ISync {
        LuaContext luaContext;
        bool enabled = false;
        bool playStarted = false;
        WorldRepository *worldRepository = nullptr;

    public:
        LuaService() = default;

        ~LuaService() override = default;

        std::vector<Dependency> getDependencies() override {
            return {{"WorldRepository", &worldRepository}};
        }

        void onInitialize() override;

        void dispose() override;

        void onSync() override;

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
    };
} // Metal

#endif //METAL_ENGINE_LUASERVICE_H
