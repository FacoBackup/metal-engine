#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <stb_image.h>
#include <dwmapi.h>
#include <algorithm>
#include "WindowService.h"
#include "../ApplicationContext.h"
#include "VulkanContext.h"
#include "editor/service/ThemeService.h"

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
            SDL_PointInRect(pt, &rects.minimizeButton) ||
            SDL_PointInRect(pt, &rects.saveButton) ||
            SDL_PointInRect(pt, &rects.projectNameButton) ||
            SDL_PointInRect(pt, &rects.playStopButton) ||
            SDL_PointInRect(pt, &rects.historyButton)) {
            return SDL_HITTEST_NORMAL;
        }

        for (const auto &dockRect : rects.dockAdders) {
            if (SDL_PointInRect(pt, &dockRect)) {
                return SDL_HITTEST_NORMAL;
            }
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
                SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_MAXIMIZED;

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

        // Load and set window icon
        int width, height, channels;
        unsigned char *pixels = stbi_load("resources/logo.png", &width, &height, &channels, 4);
        if (pixels) {
            SDL_Surface *icon = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA32, pixels, width * 4);
            if (icon) {
                SDL_SetWindowIcon(window, icon);
                SDL_DestroySurface(icon);
            }
        }

        // Enable Windows 11 rounded corners and title bar color
        HWND hwnd = (HWND) SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER,
                                                  NULL);
        if (hwnd) {
            DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
            DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

            if (pixels) {
                // Windows icons expect BGRA. We'll swap the channels in-place.
                for (int i = 0; i < width * height; i++) {
                    unsigned char temp = pixels[i * 4];
                    pixels[i * 4] = pixels[i * 4 + 2];
                    pixels[i * 4 + 2] = temp;
                }

                HBITMAP hbmColor = CreateBitmap(width, height, 1, 32, pixels);
                HBITMAP hbmMask = CreateCompatibleBitmap(GetDC(NULL), width, height);

                ICONINFO ii = {0};
                ii.fIcon = TRUE;
                ii.hbmColor = hbmColor;
                ii.hbmMask = hbmMask;

                HICON hIcon = CreateIconIndirect(&ii);
                if (hIcon) {
                    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM) hIcon);
                    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM) hIcon);
                }

                DeleteObject(hbmColor);
                DeleteObject(hbmMask);
            }
        }
        if (pixels) {
            stbi_image_free(pixels);
        }
    }

    const std::vector<const char *> &WindowService::getInstanceExtensions() const {
        return instance_extensions;
    }
}
