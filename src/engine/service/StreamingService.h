#ifndef STREAMINGREPOSITORY_H
#define STREAMINGREPOSITORY_H
#include <unordered_map>
#include <string>
#include <chrono>

#include "../../common/AbstractResourceService.h"
#include "../../common/AbstractCoreRepository.h"

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace Metal {
    struct MeshInstance;
    struct TextureInstance;
    struct SVOInstance;

    class StreamingService final : public AbstractRuntimeComponent {
        std::unordered_map<std::string, long long> lastUse{};
        TimePoint sinceLastCleanup;

    public:
        void onSync() override;
    };
} // Metal

#endif //STREAMINGREPOSITORY_H
