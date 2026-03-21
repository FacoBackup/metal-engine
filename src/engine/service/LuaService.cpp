#include "LuaService.h"
#include <iostream>

namespace Metal {

    void LuaService::onInitialize() {
        lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table, sol::lib::string, sol::lib::debug);
        std::cout << "LuaService: Lua initialized." << std::endl;
    }

    void LuaService::dispose() {
        // Lua state is handled by sol::state destructor
    }

    bool LuaService::executeString(const std::string& script) {
        auto result = lua.safe_script(script, sol::script_pass_on_error);
        if (!result.valid()) {
            sol::error err = result;
            std::cerr << "LuaService: Script error: " << err.what() << std::endl;
            return false;
        }
        return true;
    }

    bool LuaService::executeFile(const std::string& path) {
        auto result = lua.safe_script_file(path, sol::script_pass_on_error);
        if (!result.valid()) {
            sol::error err = result;
            std::cerr << "LuaService: File script error (" << path << "): " << err.what() << std::endl;
            return false;
        }
        return true;
    }

} // Metal
