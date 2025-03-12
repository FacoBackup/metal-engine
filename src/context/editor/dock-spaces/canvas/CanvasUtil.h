#ifndef CANVASUTIL_H
#define CANVASUTIL_H
#include <imgui.h>
#include "../../../../dto/scripting/IO.h"

#define IO_CIRCLE_RADIUS 5.0f
#define IO_CIRCLE_RADIUS_2 IO_CIRCLE_RADIUS * 2.f
#define IO_CIRCLE_RADIUS_3 IO_CIRCLE_RADIUS * 3.f
#define NODE_HEADER_SIZE 25.f
#define NODE_BORDER_RADIUS 4.0f

namespace Metal::CanvasUtil {
    inline ImU32 GetColorForIOType(IOType type) {
        switch (type) {
            case IOType::IO_BOOLEAN: return IM_COL32(255, 100, 100, 255);
            case IOType::IO_FLOW: return IM_COL32(255, 255, 255, 255);
            // case IOType::String: return IM_COL32(100, 100, 255, 255);
            default: return IM_COL32(200, 200, 200, 255);
        }
    }
}
#endif //CANVASUTIL_H
