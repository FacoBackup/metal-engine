#ifndef EDITORMODE_H
#define EDITORMODE_H
#include <array>

namespace Metal::EditorMode {
    enum EditorMode {
        TRANSFORM,
        PAINT
    };

    static const char *Names = "Transform\0Painting";

    static EditorMode ValueOfIndex(const int option) {
        if (option == 0) {
            return EditorMode::TRANSFORM;
        }
        return EditorMode::PAINT;
    }

    static int IndexOfValue(const EditorMode mode) {
        if (mode == EditorMode::TRANSFORM) {
            return 0;
        }
        return 1;
    }
}
#endif //EDITORMODE_H
