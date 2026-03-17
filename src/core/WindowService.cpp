#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL_system.h>
#include <dwmapi.h>
#include "WindowService.h"
#include "../ApplicationContext.h"
#include "../common/VulkanUtils.h"
#include "VulkanContext.h"
#include "../editor/service/ThemeService.h"

namespace Metal {
    SDL_Window *WindowService::getWindow() const {
        return window;
    }

    bool WindowService::isValidContext() const {
        return validContext;
    }

    void WindowService::disposeManually() {
        if (window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
        SDL_Quit();
    }

    void WindowService::onInitialize() {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
            SDL_Log("SDL_Init Error: %s", SDL_GetError());
            validContext = false;
            return;
        }

        SDL_WindowFlags window_flags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
        window = SDL_CreateWindow(ENGINE_NAME, 1280, 720, window_flags);

        if (!window) {
            SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
            validContext = false;
            return;
        }

        uint32_t extensions_count = 0;
        const char *const *sdl_extensions = SDL_Vulkan_GetInstanceExtensions(&extensions_count);
        for (uint32_t i = 0; i < extensions_count; i++) {
            instance_extensions.push_back(sdl_extensions[i]);
        }

        SDL_ShowWindow(window);

        // Enable Windows 11 rounded corners and title bar color
        HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
        if (hwnd) {
            DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
            DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

            if (themeService) {
                COLORREF color = RGB(
                    static_cast<int>(themeService->palette1.x * 255),
                    static_cast<int>(themeService->palette1.y * 255),
                    static_cast<int>(themeService->palette1.z * 255)
                );
                DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &color, sizeof(color));

                COLORREF textColor = RGB(255, 255, 255);
                DwmSetWindowAttribute(hwnd, DWMWA_TEXT_COLOR, &textColor, sizeof(textColor));
            }
        }
    }

    const std::vector<const char *> &WindowService::getInstanceExtensions() const {
        return instance_extensions;
    }
}
