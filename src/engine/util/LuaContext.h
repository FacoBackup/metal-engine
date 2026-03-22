#ifndef METAL_ENGINE_LUACONTEXT_H
#define METAL_ENGINE_LUACONTEXT_H

#include <sol/sol.hpp>
#include <string>

namespace Metal {
    struct WorldRepository;

    class LuaContext {
    public:
        void initialize(WorldRepository* worldRepository);
        
        sol::state& getState() { return lua; }

        const sol::state& getState() const { return lua; }

    private:
        sol::state lua;
    };
}

#endif //METAL_ENGINE_LUACONTEXT_H
