#ifndef METAL_ENGINE_LUACONTEXT_H
#define METAL_ENGINE_LUACONTEXT_H

#include <sol/sol.hpp>
#include <string>

#include "common/IContextMember.h"
#include "common/IInit.h"

namespace Metal {
    struct WorldRepository;
    class CameraService;
    class EngineContext;
    class TransformService;

    class LuaContext : public IContextMember, public IInit {
        WorldRepository *worldRepository = nullptr;
        CameraService *cameraService = nullptr;
        EngineContext *engineContext = nullptr;
        TransformService *transformService = nullptr;

    public:
        void onInitialize() override;

        sol::state &getState() { return lua; }

        const sol::state &getState() const { return lua; }

        std::vector<Dependency> getDependencies() override {
            return {
                {"WorldRepository", &worldRepository},
                {"CameraService", &cameraService},
                {"EngineContext", &engineContext},
                {"TransformService", &transformService}
            };
        }

    private:
        sol::state lua;
    };
}

#endif //METAL_ENGINE_LUACONTEXT_H
