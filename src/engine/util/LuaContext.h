#ifndef METAL_ENGINE_LUACONTEXT_H
#define METAL_ENGINE_LUACONTEXT_H

#include <sol/sol.hpp>
#include <string>

namespace Metal {
    struct WorldRepository;

    class EngineContext;
    class TransformService;
    struct ScriptComponent;

    class LuaContext {
    public:
        void initialize(WorldRepository* worldRepository, EngineContext* engineContext, TransformService* transformService);
        
        sol::state& getState() { return lua; }

        const sol::state& getState() const { return lua; }

    private:
        sol::state lua;
    };
}

#endif //METAL_ENGINE_LUACONTEXT_H
