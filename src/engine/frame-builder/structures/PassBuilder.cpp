#include "PassBuilder.h"
#include "../../render-pass/impl/PostProcessingPass.h"
#include "../../passes/impl/HWRayTracingPass.h"
#include "../../passes/impl/TemporalAccumulationPass.h"
#include "../../passes/impl/SpatialFilterPass.h"
#include "../../render-pass/impl/tools/SelectionOutlinePass.h"
#include "../../render-pass/impl/tools/GridPass.h"
#include "../../render-pass/impl/tools/IconsPass.h"
#include "../../render-pass/impl/tools/SelectionIDPass.h"

namespace Metal {
    RuntimeResource* PassBuilder::build() {
        switch (passType) {
            case POST_PROCESSING:
                return new PostProcessingPass(id);
            case RAY_TRACING:
                return new HWRayTracingPass(id);
            case TEMPORAL_ACCUMULATION:
                return new TemporalAccumulationPass(id);
            case SPATIAL_FILTER:
                return new SpatialFilterPass(id);
            case SELECTION_OUTLINE:
                return new SelectionOutlinePass(id);
            case GRID:
                return new GridPass(id);
            case ICONS:
                return new IconsPass(id);
            case SELECTION_ID:
                return new SelectionIDPass(id);
            default:
                return nullptr;
        }
    }
}
