#include "MethodField.h"

#include <imgui.h>

namespace Metal {
    MethodField::MethodField(InspectedMethod &method) : method(method) {
    }

    void MethodField::onSync() {
        if (ImGui::Button(method.nameWithId.c_str())) {
            method.callback();
        }
    }
}
