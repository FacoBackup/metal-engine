#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <dwmapi.h>
#include <algorithm>
#include "WindowService.h"
#include "../ApplicationContext.h"
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

    void WindowService::close() const {
        SDL_Event event;
        event.type = SDL_EVENT_QUIT;
        SDL_PushEvent(&event);
    }

    bool WindowService::isMaximized() const {
        return SDL_GetWindowFlags(window) & SDL_WINDOW_MAXIMIZED;
    }

    void WindowService::maximize() const {
        if (isMaximized()) {
            SDL_RestoreWindow(window);
        } else {
            SDL_MaximizeWindow(window);
        }
    }

    void WindowService::minimize() const {
        SDL_MinimizeWindow(window);
    }

    SDL_HitTestResult SDLCALL window_hit_test(SDL_Window *win, const SDL_Point *pt, void *data) {
        auto *service = static_cast<WindowService *>(data);
        const auto &rects = service->getWindowControlRects();

        // Check window controls (buttons) first to ensure they aren't part of the drag area
        if (SDL_PointInRect(pt, &rects.closeButton) ||
            SDL_PointInRect(pt, &rects.maximizeButton) ||
            SDL_PointInRect(pt, &rects.minimizeButton)) {
            return SDL_HITTEST_NORMAL;
        }

        // Draggable area
        if (SDL_PointInRect(pt, &rects.dragArea)) {
            return SDL_HITTEST_DRAGGABLE;
        }

        // Resizing at edges
        int w, h;
        SDL_GetWindowSize(win, &w, &h);
        constexpr int border = 5;

        if (pt->y < border) {
            if (pt->x < border) return SDL_HITTEST_RESIZE_TOPLEFT;
            if (pt->x > w - border) return SDL_HITTEST_RESIZE_TOPRIGHT;
            return SDL_HITTEST_RESIZE_TOP;
        }
        if (pt->y > h - border) {
            if (pt->x < border) return SDL_HITTEST_RESIZE_BOTTOMLEFT;
            if (pt->x > w - border) return SDL_HITTEST_RESIZE_BOTTOMRIGHT;
            return SDL_HITTEST_RESIZE_BOTTOM;
        }
        if (pt->x < border) return SDL_HITTEST_RESIZE_LEFT;
        if (pt->x > w - border) return SDL_HITTEST_RESIZE_RIGHT;

        return SDL_HITTEST_NORMAL;
    }

    void WindowService::onInitialize() {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
            SDL_Log("SDL_Init Error: %s", SDL_GetError());
            validContext = false;
            return;
        }

        SDL_WindowFlags window_flags =
                SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_BORDERLESS;
        window = SDL_CreateWindow(ENGINE_NAME, 1280, 720, window_flags);

        if (!window) {
            SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
            validContext = false;
            return;
        }

        SDL_SetWindowHitTest(window, window_hit_test, this);

        uint32_t extensions_count = 0;
        const char *const *sdl_extensions = SDL_Vulkan_GetInstanceExtensions(&extensions_count);
        for (uint32_t i = 0; i < extensions_count; i++) {
            instance_extensions.push_back(sdl_extensions[i]);
        }

        SDL_ShowWindow(window);

        // Enable Windows 11 rounded corners and title bar color
        HWND hwnd = (HWND) SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER,
                                                  NULL);
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
