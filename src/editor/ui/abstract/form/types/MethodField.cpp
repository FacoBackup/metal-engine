#include "MethodField.h"
#include <algorithm>
#include <imgui.h>

namespace Metal {
    MethodField::MethodField(InspectedMethod &method) : method(method) {
    }

    void MethodField::onSync() {
        if (ImGui::Button(method.nameWithId.c_str())) {
            method.callback();
        }
    }

    bool MethodField::isVisible() const {
        if (!filter || filter->empty()) return true;
        std::string lowerName = method.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        return lowerName.find(*filter) != std::string::npos;
    }
}
