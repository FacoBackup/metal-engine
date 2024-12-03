#ifndef METAL_ENGINE_RENDERERCOREDATA_H
#define METAL_ENGINE_RENDERERCOREDATA_H

#include "VulkanCoreData.h"
#include "imgui_impl_vulkan.h"

namespace Metal {
    struct RendererCoreData {
        ImGui_ImplVulkanH_Window g_MainWindowData;
        int g_MinImageCount = 2;
        bool g_SwapChainRebuild = false;
        VulkanCoreData coreData;
    };
}
#endif
