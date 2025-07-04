#ifndef STREAMINGREPOSITORY_H
#define STREAMINGREPOSITORY_H
#include <chrono>
#include <unordered_map>
#include <string>

#include "../../common/AbstractRuntimeComponent.h"

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace Metal {
    struct LevelOfDetail;
    struct TextureInstance;
    struct MaterialInfo;

    class StreamingService final : public AbstractRuntimeComponent {
        std::unordered_map<std::string, unsigned int> tries{};
        std::unordered_map<std::string, long long> lastUse{};
        TimePoint sinceLastCleanup;

    public:
        MaterialInfo *streamMaterial(const std::string &id);

        TextureInstance *streamTexture(const std::string &id, const LevelOfDetail &lod);

        void onSync() override;
    };
} // Metal

#endif //STREAMINGREPOSITORY_H
