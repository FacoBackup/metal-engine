#ifndef SHADERSERVICE_H
#define SHADERSERVICE_H
#include <iostream>
#include <glslang/Include/glslang_c_shader_types.h>
#include <vulkan/vulkan_core.h>

#include "../runtime/ShaderModule.h"
#include "../ApplicationContext.h"

namespace Metal {
    class ShaderUtil final {
        static void CheckShaderCompilation(glslang_shader_t *shader);

        static bool CompileShader(const VulkanContext &context, glslang_stage_t stage, const char *pShaderCode,
                                  ShaderModule *shaderModule);

        static glslang_stage_t ShaderStageFromFilename(const char *pFilename);

    public:
        static VkShaderModule CreateShaderModule(ApplicationContext &context, const char *pFilename);
    };
} // Metal

#endif
