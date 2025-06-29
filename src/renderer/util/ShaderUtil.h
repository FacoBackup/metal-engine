#ifndef SHADERSERVICE_H
#define SHADERSERVICE_H
#include <iostream>
#include <glslang/Include/glslang_c_shader_types.h>
#include <vulkan/vulkan_core.h>

#include "../data/ShaderModule.h"

namespace Metal
{
    class ShaderUtil final
    {
        static void CheckShaderCompilation(glslang_shader_t* shader);

        static bool CompileShader(glslang_stage_t stage, const char* pShaderCode,
                                  ShaderModule* shaderModule);

        static glslang_stage_t ShaderStageFromFilename(const char* pFilename);

        static std::string ProcessIncludes(const std::string& input);

        static std::string ProcessShader(const std::string& file);

    public:
        static VkShaderModule CreateShaderModule( const std::string& pFilename);
    };
} // Metal

#endif
