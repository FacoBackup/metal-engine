#include "LuaService.h"
#include <iostream>
#include <entt/entt.hpp>
#include "../repository/WorldRepository.h"
#include "../dto/ScopedScriptComponent.h"
#include "../dto/GlobalScriptComponent.h"
#include "../dto/MetadataComponent.h"
#include "../../ApplicationEventContext.h"

namespace Metal {
    void LuaService::onInitialize() {
        luaContext.initialize(worldRepository);
        auto &lua = luaContext.getState();

        eventListener([this, &lua](const Event &) {
            enabled = true;
            playStarted = true;

            // Execute all onCreate scripts
            auto view = worldRepository->registry.view<ScopedScriptComponent>();
            for (auto entity: view) {
                auto &comp = view.get<ScopedScriptComponent>(entity);
                if (!comp.onCreatePath.empty()) {
                    lua["this_entity"] = entity;
                    executeFile(comp.onCreatePath);
                }
            }

            auto globalView = worldRepository->registry.view<GlobalScriptComponent>();
            for (auto entity: globalView) {
                auto &comp = globalView.get<GlobalScriptComponent>(entity);
                if (!comp.scriptPath.empty()) {
                    lua["this_entity"] = entity;
                    executeFile(comp.scriptPath);
                }
            }
        }, "Play");

        eventListener([this, &lua](const Event &) {
            enabled = false;

            // Execute all onDestroy scripts
            auto view = worldRepository->registry.view<ScopedScriptComponent>();
            for (auto entity: view) {
                auto &comp = view.get<ScopedScriptComponent>(entity);
                if (!comp.onDestroyPath.empty()) {
                    lua["this_entity"] = entity;
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

        auto &lua = luaContext.getState();
        {
            auto view = worldRepository->registry.view<ScopedScriptComponent>();
            for (auto entity: view) {
                auto &comp = view.get<ScopedScriptComponent>(entity);
                if (!comp.onUpdatePath.empty()) {
                    lua["this_entity"] = entity;
                    executeFile(comp.onUpdatePath);
                }
            }
        }
        {
            auto view = worldRepository->registry.view<GlobalScriptComponent>();
            for (auto entity: view) {
                auto &comp = view.get<GlobalScriptComponent>(entity);
                if (!comp.scriptPath.empty()) {
                    lua["this_entity"] = entity;
                    executeFile(comp.scriptPath);
                }
            }
        }
    }

    bool LuaService::executeString(const std::string &script) {
        auto &lua = luaContext.getState();
        auto result = lua.safe_script(script, sol::script_pass_on_error);
        if (!result.valid()) {
            sol::error err = result;
            std::cerr << "LuaService: Script error: " << err.what() << std::endl;
            return false;
        }
        return true;
    }

    bool LuaService::executeFile(const std::string &path) {
        auto &lua = luaContext.getState();
        auto result = lua.safe_script_file(path, sol::script_pass_on_error);
        if (!result.valid()) {
            sol::error err = result;
            std::cerr << "LuaService: File script error (" << path << "): " << err.what() << std::endl;
            return false;
        }
        return true;
    }
} // Metal
