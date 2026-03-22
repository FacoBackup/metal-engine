#ifndef METAL_ENGINE_SHORTCUTDTO_H
#define METAL_ENGINE_SHORTCUTDTO_H

#include <string>
#include <functional>
#include <vector>
#include <imgui.h>

namespace Metal {
    struct ShortcutDTO {
        std::string name;
        ImGuiKeyChord keyChord;
        std::function<void()> callback;
        bool isDown = false;

        ShortcutDTO(std::string name, ImGuiKeyChord keyChord, std::function<void()> callback, const bool isDown = false)
            : name(std::move(name)), keyChord(keyChord), callback(std::move(callback)), isDown(isDown) {}
    };
}

#endif
