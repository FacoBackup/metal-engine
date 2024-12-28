#ifndef STREAMINGREPOSITORY_H
#define STREAMINGREPOSITORY_H
#include <unordered_map>
#include <string>

#include "../abstract/AbstractCoreRepository.h"


namespace Metal {
    struct SVOInstance;
}

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace Metal {
    struct LevelOfDetail;
    struct MeshInstance;
    struct TextureInstance;

    class StreamingRepository final : public AbstractRuntimeComponent {
        std::unordered_map<std::string, unsigned int> tries{};
        TimePoint sinceLastCleanup;

    public:
        explicit StreamingRepository(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        MeshInstance *streamMesh(const std::string &id, const LevelOfDetail &lod);

        SVOInstance *streamSVO(const std::string &id, const LevelOfDetail &lod);

        TextureInstance *streamTexture(const std::string &id, const LevelOfDetail &lod);

        void onSync() override;
    };
} // Metal

#endif //STREAMINGREPOSITORY_H
