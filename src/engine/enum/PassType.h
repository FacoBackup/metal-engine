#ifndef PASS_TYPE_H
#define PASS_TYPE_H

namespace Metal {
    enum PassType {
        RAY_TRACING,
        ACCUMULATION,
        TEMPORAL_ACCUMULATION,
        SPATIAL_FILTER,
        POST_PROCESSING,
        SELECTION_OUTLINE,
        GRID,
        ICONS,
        SELECTION_ID
    };
}

#endif //PASS_TYPE_H
