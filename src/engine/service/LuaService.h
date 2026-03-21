#ifndef METAL_ENGINE_LUASERVICE_H
#define METAL_ENGINE_LUASERVICE_H

#include "common/IService.h"
#include "common/IInit.h"
#include "common/IDisposable.h"
#include <sol/sol.hpp>
#include <string>

namespace Metal {

    class LuaService final : public IService, public IInit, public IDisposable {
    private:
        sol::state lua;

    public:
        LuaService() = default;
        ~LuaService() override = default;

        std::vector<Dependency> getDependencies() override {
            return {};
        }

        void onInitialize() override;
        void dispose() override;

        /**
         * Executes a lua script string.
         * @param script The lua script code.
         * @return true if execution was successful.
         */
        bool executeString(const std::string& script);

        /**
         * Executes a lua script file.
         * @param path The path to the lua file.
         * @return true if execution was successful.
         */
        bool executeFile(const std::string& path);

        /**
         * Access the lua state directly for bindings.
         */
        sol::state& getState() { return lua; }
    };

} // Metal

#endif //METAL_ENGINE_LUASERVICE_H
