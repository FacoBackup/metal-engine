#ifndef METAL_ENGINE_VIDEOEXPORTERSERVICE_H
#define METAL_ENGINE_VIDEOEXPORTERSERVICE_H
#include "../../common/AbstractRuntimeComponent.h"
#include <string>

namespace Metal {
    class VideoExporterService : public AbstractRuntimeComponent {
        bool isFirstBakeLoop = false;
        int frameCount = 0;
        std::string frameOutputDirectory;

        void cleanupFFmpeg();

    public:
        VideoExporterService(bool is_first_bake_loop)
            : AbstractRuntimeComponent(),
              isFirstBakeLoop(is_first_bake_loop) {
        }

        void exportToVideo();

        void finishExportToVideo();

        void onSync() override;
    };
} // Metal

#endif
