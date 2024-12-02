#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_metal.h"
#include <stdio.h>

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>

#import "../IUIRenderer.h"

#ifndef METALRENDERER_H
#define METALRENDERER_H
#pragma once
namespace backends::metal {
    class MetalRenderer : public IUIRenderer {
    public:
        MetalRenderer();

        ~MetalRenderer() override;

        void start() override;

        void shutdown() override;

    private:

        void newFrame() override;

        void render() override;

        id <MTLDevice> device;
        id <MTLCommandQueue> commandQueue;
        MTLRenderPassDescriptor *renderPassDescriptor;
        id <MTLRenderCommandEncoder> renderEncoder;
        id <MTLCommandBuffer> commandBuffer;
        id <CAMetalDrawable> drawable;
        GLFWwindow *window;
        NSWindow *nswin;
        CAMetalLayer *layer;
        float clear_color[4] = {0.f, 0.f, 0.f, 1.00f};
    };
}
#endif