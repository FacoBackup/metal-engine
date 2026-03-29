#ifndef STREAMINGREPOSITORY_H
#define STREAMINGREPOSITORY_H
#include <unordered_map>
#include <string>
#include <chrono>

#include "../../common/IService.h"
#include "../../common/ISync.h"

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace Metal {
    struct MeshInstance;
    struct TextureInstance;
    class EngineContext;
    struct WorldRepository;
    class MeshService;
    class TextureService;

    class StreamingService final : public IService, public ISync {
        std::unordered_map<std::string, long long> lastUse{};
        TimePoint sinceLastCleanup;

        EngineContext *engineContext = nullptr;
        WorldRepository *worldRepository = nullptr;
        MeshService *meshService = nullptr;
        TextureService *textureService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"EngineContext", &engineContext},
                {"WorldRepository", &worldRepository},
                {"MeshService", &meshService},
                {"TextureService", &textureService}
            };
        }

        void onSync() override;
    };
} // Metal

#endif //STREAMINGREPOSITORY_H
