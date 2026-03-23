#ifndef THEMESERVICE_H
#define THEMESERVICE_H
#include <imgui.h>
#include <glm/vec3.hpp>
#include <nlohmann/json.hpp>
#include <string>

#include "common/IService.h"
#include "common/ISync.h"

namespace Metal {
    class VulkanContext;
    struct EditorRepository;
    class vulkanContext;

    class ThemeService final : public IService, public ISync {
        EditorRepository *editorRepository = nullptr;
        VulkanContext *vulkanContext = nullptr;
        nlohmann::json themeData;
        bool themeLoaded = false;

        void loadTheme();

        static ImVec4 hexToRGBA(const std::string &hex);

    public:
        ImVec4 neutralPalette{};
        ImVec4 palette0{};
        ImVec4 palette1{};
        ImVec4 palette2{};
        ImVec4 palette3{};
        ImVec4 palette4{};
        ImVec4 palette5{};
        ImVec4 palette6{};
        ImVec4 textDisabled{};
        static glm::vec3 BACKGROUND_COLOR;
        bool previousTheme = false;
        float prevLength = 0.0f;

        void setDarkMode();

        void setLightMode();

        void onSync() override;

        std::vector<Dependency> getDependencies() override {
            return {
                {"EditorRepository", &editorRepository},
                {"VulkanContext", &vulkanContext}
            };
        }
    };
} // Metal

#endif
