#include "EditorRepository.h"

#include "../../common/interface/Icons.h"

namespace Metal {
    const char * EditorRepository::getIcon() {
        return Icons::settings.c_str();
    }

    const char * EditorRepository::getTitle() {
        return "Editor";
    }
}
