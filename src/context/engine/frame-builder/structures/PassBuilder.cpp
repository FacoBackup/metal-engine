#include "PassBuilder.h"
#include "../../render-pass/impl/PostProcessingPass.h"
#include "../../compute-pass/impl/HWRayTracingPass.h"
#include "../../compute-pass/impl/AccumulationPass.h"
#include "../../compute-pass/impl/TemporalAccumulationPass.h"
#include "../../compute-pass/impl/SpatialFilterPass.h"
#include "../../render-pass/impl/tools/SelectedDotPass.h"
#include "../../render-pass/impl/tools/GridPass.h"
#include "../../render-pass/impl/tools/IconsPass.h"

namespace Metal {
    RuntimeResource* PassBuilder::build() {
        switch (passType) {
            case POST_PROCESSING:
                return new PostProcessingPass(id);
            case RAY_TRACING:
                return new HWRayTracingPass(id);
            case ACCUMULATION:
                return new AccumulationPass(id);
            case TEMPORAL_ACCUMULATION:
                return new TemporalAccumulationPass(id);
            case SPATIAL_FILTER:
                return new SpatialFilterPass(id);
            case SELECTED_DOT:
                return new SelectedDotPass(id);
            case GRID:
                return new GridPass(id);
            case ICONS:
                return new IconsPass(id);
            default:
                return nullptr;
        }
    }
}
