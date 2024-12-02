#import "MetalRenderer.h"

namespace backends::metal {
    MetalRenderer::~MetalRenderer() {
        shutdown();
    }

    void MetalRenderer::start() {
        while (!glfwWindowShouldClose(window)) {
            @autoreleasepool {
                newFrame();
                render();
            }
        }
    }

    void MetalRenderer::newFrame() {
        glfwPollEvents();

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        layer.drawableSize = CGSizeMake(width, height);
        drawable = [layer nextDrawable];
        commandBuffer = [commandQueue commandBuffer];
        renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clear_color[0] * clear_color[3],
                                                                                clear_color[1] * clear_color[3],
                                                                                clear_color[2] * clear_color[3],
                                                                                clear_color[3]);
        renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
        renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
        renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
        renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
        [renderEncoder pushDebugGroup:@"ImGui demo"];

        ImGui_ImplMetal_NewFrame(renderPassDescriptor);
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void MetalRenderer::render() {
        // START
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin(
                "Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float *) &clear_color); // Edit 3 floats representing a color

        if (ImGui::Button(
                "Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::End();
        // END

        // Rendering
        ImGui::Render();
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, renderEncoder);

        [renderEncoder popDebugGroup];
        [renderEncoder endEncoding];

        [commandBuffer presentDrawable:drawable];
        [commandBuffer commit];
    }

    void MetalRenderer::shutdown() {
        ImGui_ImplMetal_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();


        glfwDestroyWindow(window);
        glfwTerminate();
    }

    static void glfw_error_callback(int error, const char *description) {
        fprintf(stderr, "Glfw Error %d: %s\n", error, description);
    }

    MetalRenderer::MetalRenderer() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

        ImGui::StyleColorsDark();

        // Setup window
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit()) {
            throw "Could not start window";
        }

        // Create window with graphics context
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+Metal example", nullptr, nullptr);
        if (window == nullptr) {
            throw "Could not start window";
        }

        device = MTLCreateSystemDefaultDevice();
        commandQueue = [device newCommandQueue];

        // Setup Platform/Renderer backends-impl
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplMetal_Init(device);

        nswin = glfwGetCocoaWindow(window);
        layer = [CAMetalLayer layer];
        layer.device = device;
        layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        nswin.contentView.layer = layer;
        nswin.contentView.wantsLayer = YES;

        renderPassDescriptor = [MTLRenderPassDescriptor new];
    }

}
