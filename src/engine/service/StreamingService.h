#ifndef STREAMINGREPOSITORY_H
#define STREAMINGREPOSITORY_H
#include <unordered_map>
#include <string>
#include <chrono>

#include "../../common/AbstractRuntimeComponent.h"

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace Metal {
    struct MeshInstance;
    struct TextureInstance;
    struct SVOInstance;
    class EngineContext;
    struct WorldRepository;
    class MeshService;
    class TextureService;
    class VoxelService;

    class StreamingService final : public AbstractRuntimeComponent {
        std::unordered_map<std::string, long long> lastUse{};
        TimePoint sinceLastCleanup;

        EngineContext &engineContext;
        WorldRepository &worldRepository;
        MeshService &meshService;
        TextureService &textureService;
        VoxelService &voxelService;

    public:
        StreamingService(EngineContext &engineContext, WorldRepository &worldRepository, MeshService &meshService, TextureService &textureService, VoxelService &voxelService)
            : engineContext(engineContext), worldRepository(worldRepository), meshService(meshService), textureService(textureService), voxelService(voxelService) {}

        void onSync() override;
    };
} // Metal

#endif //STREAMINGREPOSITORY_H
