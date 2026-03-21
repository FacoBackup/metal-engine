#include "LuaService.h"
#include <iostream>
#include "../repository/WorldRepository.h"
#include "../dto/ScopedScriptComponent.h"
#include "../dto/GlobalScriptComponent.h"
#include "../../ApplicationEventContext.h"

namespace Metal {
    void LuaService::onInitialize() {
        lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table, sol::lib::string,
                           sol::lib::debug);
        std::cout << "LuaService: Lua initialized." << std::endl;

        eventListener([this](const Event &) {
            enabled = true;
            playStarted = true;

            // Execute all onCreate scripts
            auto view = worldRepository->registry.view<ScopedScriptComponent>();
            for (auto entity: view) {
                auto &comp = view.get<ScopedScriptComponent>(entity);
                if (!comp.onCreatePath.empty()) {
                    executeFile(comp.onCreatePath);
                }
            }

            auto globalView = worldRepository->registry.view<GlobalScriptComponent>();
            for (auto entity: globalView) {
                auto &comp = globalView.get<GlobalScriptComponent>(entity);
                if (!comp.scriptPath.empty()) {
                    executeFile(comp.scriptPath);
                }
            }
        }, "Play");

        eventListener([this](const Event &) {
            enabled = false;

            // Execute all onDestroy scripts
            auto view = worldRepository->registry.view<ScopedScriptComponent>();
            for (auto entity: view) {
                auto &comp = view.get<ScopedScriptComponent>(entity);
                if (!comp.onDestroyPath.empty()) {
                    executeFile(comp.onDestroyPath);
                }
            }

            playStarted = false;
        }, "Stop");
    }

    void LuaService::dispose() {
        // Lua state is handled by sol::state destructor
    }

    void LuaService::onSync() {
        if (!enabled) return;

        {
            auto view = worldRepository->registry.view<ScopedScriptComponent>();
            for (auto entity: view) {
                auto &comp = view.get<ScopedScriptComponent>(entity);
                if (!comp.onUpdatePath.empty()) {
                    executeFile(comp.onUpdatePath);
                }
            }
        }
        {
            auto view = worldRepository->registry.view<GlobalScriptComponent>();
            for (auto entity: view) {
                auto &comp = view.get<GlobalScriptComponent>(entity);
                if (!comp.scriptPath.empty()) {
                    executeFile(comp.scriptPath);
                }
            }
        }
    }

    bool LuaService::executeString(const std::string &script) {
        auto result = lua.safe_script(script, sol::script_pass_on_error);
        if (!result.valid()) {
            sol::error err = result;
            std::cerr << "LuaService: Script error: " << err.what() << std::endl;
            return false;
        }
        return true;
    }

    bool LuaService::executeFile(const std::string &path) {
        auto result = lua.safe_script_file(path, sol::script_pass_on_error);
        if (!result.valid()) {
            sol::error err = result;
            std::cerr << "LuaService: File script error (" << path << "): " << err.what() << std::endl;
            return false;
        }
        return true;
    }
} // Metal
