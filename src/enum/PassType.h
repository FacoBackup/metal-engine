#ifndef PASS_TYPE_H
#define PASS_TYPE_H

namespace Metal {
    enum PassType {
        RAY_TRACING,
        ACCUMULATION,
        TEMPORAL_ACCUMULATION,
        SPATIAL_FILTER,
        POST_PROCESSING,
        SELECTED_DOT,
        GRID,
        ICONS
    };
}

#endif //PASS_TYPE_H
