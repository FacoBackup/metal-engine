#include "LuaService.h"
#include <iostream>
#include <entt/entt.hpp>
#include "../repository/WorldRepository.h"
#include "../dto/ScriptComponent.h"
#include "../EngineContext.h"
#include <unordered_map>

#include "ApplicationContext.h"
#include "common/LoggerUtil.h"

namespace Metal {
    void LuaService::onInitialize() {
        ctx->injectDependencies(&luaContext);
        luaContext.onInitialize();

        // Registry listeners for automatic lifecycle management
        worldRepository->registry.on_construct<ScriptComponent>().connect<&LuaService::onScriptComponentAdded>(this);
        worldRepository->registry.on_update<ScriptComponent>().connect<&LuaService::onScriptComponentUpdated>(this);
        worldRepository->registry.on_destroy<ScriptComponent>().connect<&LuaService::onScriptComponentRemoved>(this);

        eventListener([this](const Event &) {
            std::lock_guard<std::mutex> lock(luaMutex);
            enabled = true;
            playStarted = true;
            activeUpdateScripts.clear();
            activeAsyncUpdateScripts.clear();

            auto view = worldRepository->registry.view<ScriptComponent>();
            for (auto entity: view) {
                auto &comp = view.get<ScriptComponent>(entity);
                loadScript(comp, entity);
            }
        }, "Play");

        // Stop event
        eventListener([this](const Event &) {
            std::lock_guard<std::mutex> lock(luaMutex);
            enabled = false;

            auto view = worldRepository->registry.view<ScriptComponent>();
            for (auto entity: view) {
                auto &comp = view.get<ScriptComponent>(entity);
                if (comp.instance.valid() && comp.instance["onDestroy"].valid()) {
                    auto res = comp.instance["onDestroy"](comp.instance);
                    if (!res.valid()) {
                        sol::error err = res;
                        LOG_ERROR("LuaService: onDestroy error: " + std::string(err.what()));
                    }
                }
                comp.instance = sol::lua_nil;
                comp.env = sol::lua_nil;
                comp.updateFunc = sol::lua_nil;
                comp.isErrored = false;
            }

            activeUpdateScripts.clear();
            activeAsyncUpdateScripts.clear();
            scriptCache.clear();
            playStarted = false;
        }, "Stop");
    }

    void LuaService::onScriptComponentAdded(entt::registry &registry, entt::entity entity) {
        if (!playStarted) return;
        std::lock_guard<std::mutex> lock(luaMutex);
        auto &comp = registry.get<ScriptComponent>(entity);
        loadScript(comp, entity);
    }

    void LuaService::onScriptComponentUpdated(entt::registry &registry, entt::entity entity) {
        if (!playStarted) return;
        std::lock_guard<std::mutex> lock(luaMutex);

        // Remove from active lists first
        std::erase(activeUpdateScripts, entity);
        std::erase(activeAsyncUpdateScripts, entity);

        auto &comp = registry.get<ScriptComponent>(entity);

        // Clean up old instance if it exists
        if (comp.instance.valid() && comp.instance["onDestroy"].valid()) {
            comp.instance["onDestroy"](comp.instance);
        }
        comp.instance = sol::lua_nil;
        comp.env = sol::lua_nil;
        comp.updateFunc = sol::lua_nil;
        comp.isErrored = false;

        loadScript(comp, entity);
    }

    void LuaService::onScriptComponentRemoved(entt::registry &registry, entt::entity entity) {
        std::lock_guard<std::mutex> lock(luaMutex);
        std::erase(activeUpdateScripts, entity);
        std::erase(activeAsyncUpdateScripts, entity);
        if (playStarted && registry.valid(entity) && registry.all_of<ScriptComponent>(entity)) {
            auto &comp = registry.get<ScriptComponent>(entity);
            if (comp.instance.valid() && comp.instance["onDestroy"].valid()) {
                auto res = comp.instance["onDestroy"](comp.instance);
                if (!res.valid()) {
                    sol::error err = res;
                    LOG_ERROR("LuaService: onDestroy error: " + std::string(err.what()));
                }
            }
            comp.instance = sol::lua_nil;
            comp.env = sol::lua_nil;
            comp.updateFunc = sol::lua_nil;
        }
    }

    void LuaService::loadScript(ScriptComponent &comp, entt::entity entity) {
        if (comp.scriptPath.empty()) return;
        comp.isErrored = false;

        auto &lua = luaContext.getState();
        sol::protected_function protoFunc;
        auto it = scriptCache.find(comp.scriptPath);
        if (it == scriptCache.end()) {
            auto loadResult = lua.load_file(comp.scriptPath);
            if (!loadResult.valid()) {
                sol::error err = loadResult;
                LOG_ERROR("LuaService: Failed to load script file " + comp.scriptPath + ": " + err.what());
                comp.isErrored = true;
                return;
            }
            protoFunc = loadResult;
            scriptCache[comp.scriptPath] = protoFunc;
        } else {
            protoFunc = it->second;
        }

        // Create environment for isolation
        comp.env = sol::environment(lua, sol::create, lua.globals());
        // Fix for isolation: set the environment on the function
        lua.set_function("set_env", [](sol::function f, sol::environment e) {
            sol::set_environment(e, f);
        });
        lua["set_env"](protoFunc, comp.env);

        // Run the script in its environment to get the prototype table
        sol::protected_function_result result = protoFunc();
        if (!result.valid()) {
            sol::error err = result;
            LOG_ERROR("LuaService: Failed to execute script " + comp.scriptPath + ": " + err.what());
            comp.isErrored = true;
            return;
        }

        if (result.get_type() != sol::type::table) {
            LOG_ERROR("LuaService: Script " + comp.scriptPath + " must return a table");
            comp.isErrored = true;
            return;
        }

        sol::table proto = result;
        sol::table instance = lua.create_table();

        // Set instance metatable to the prototype for method inheritance
        sol::table mt = lua.create_table();
        mt[sol::meta_function::index] = proto;
        instance[sol::metatable_key] = mt;

        instance["this_entity"] = entity;
        comp.instance = instance;

        if (instance["onCreate"].valid()) {
            sol::protected_function onCreate = instance["onCreate"];
            auto res = onCreate(instance);
            if (!res.valid()) {
                sol::error err = res;
                LOG_ERROR("LuaService: onCreate error in " + comp.scriptPath + ": " + err.what());
                comp.isErrored = true;
            }
        }

        if (instance["onUpdate"].valid()) {
            comp.updateFunc = instance["onUpdate"];
            if (comp.runAsync) {
                activeAsyncUpdateScripts.push_back(entity);
            } else {
                activeUpdateScripts.push_back(entity);
            }
        }
    }

    void LuaService::onSync() {
        if (!enabled) return;
        std::lock_guard<std::mutex> lock(luaMutex);

        // Inject delta time
        if (engineContext) {
            luaContext.getState()["deltaTime"] = engineContext->deltaTime;
        }

        for (auto it = activeUpdateScripts.begin(); it != activeUpdateScripts.end();) {
            entt::entity entity = *it;
            auto &comp = worldRepository->registry.get<ScriptComponent>(entity);

            if (comp.isErrored) {
                it++;
                continue;
            }

            if (comp.updateFunc.valid()) {
                auto res = comp.updateFunc(comp.instance);
                if (!res.valid()) {
                    sol::error err = res;
                    LOG_ERROR("LuaService: onUpdate error in " + comp.scriptPath + ": " + std::string(err.what()));
                    comp.isErrored = true;
                }
            }
            it++;
        }
    }

    void LuaService::onAsyncSync() {
        if (!enabled) return;
        std::lock_guard<std::mutex> lock(luaMutex);

        for (auto it = activeAsyncUpdateScripts.begin(); it != activeAsyncUpdateScripts.end();) {
            entt::entity entity = *it;
            auto &comp = worldRepository->registry.get<ScriptComponent>(entity);

            if (comp.isErrored) {
                it++;
                continue;
            }

            if (comp.updateFunc.valid()) {
                auto res = comp.updateFunc(comp.instance);
                if (!res.valid()) {
                    sol::error err = res;
                    LOG_ERROR("LuaService: onAsyncUpdate error in " + comp.scriptPath + ": " + std::string(err.what()));
                    comp.isErrored = true;
                }
            }
            it++;
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
