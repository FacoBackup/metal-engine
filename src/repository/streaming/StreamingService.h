#ifndef STREAMINGREPOSITORY_H
#define STREAMINGREPOSITORY_H
#include <unordered_map>
#include <string>
#include <chrono>

#include "../../service/abstract/AbstractResourceService.h"
#include "../abstract/AbstractCoreRepository.h"
#include "../../service/abstract/IStreamable.h"
#include "../abstract/RuntimeResource.h"


namespace Metal {
    struct SVOInstance;
}

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace Metal {
    struct MeshInstance;
    struct TextureInstance;
    struct MaterialInstance;

    class StreamingService final : public AbstractRuntimeComponent {
        std::unordered_map<std::string, unsigned int> tries{};
        std::unordered_map<std::string, long long> lastUse{};
        TimePoint sinceLastCleanup;

    public:
        MeshInstance *streamMesh(const std::string &id);

        MaterialInstance *streamMaterial(const std::string &id);

        SVOInstance *streamSVO(const std::string &id);

        TextureInstance *streamTexture(const std::string &id);

        void onSync() override;
    };
} // Metal

#endif //STREAMINGREPOSITORY_H
