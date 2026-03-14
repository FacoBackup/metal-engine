#ifndef SHADERSERVICE_H
#define SHADERSERVICE_H
#include <string>
#include "../dto/ShaderModule.h"
#include "../../common/AbstractRuntimeComponent.h"

namespace Metal {
    class VulkanContext;

    class ShaderService final : public AbstractRuntimeComponent {
        bool isDebugMode;
        std::string shadersDirectory;
        VulkanContext &vulkanContext;

        static void CheckShaderCompilation(glslang_shader_t *shader);

         bool CompileShader(glslang_stage_t stage, const char *pShaderCode,
                                    ShaderModule *shaderModule) const;

        static glslang_stage_t ShaderStageFromFilename(const char *pFilename);

        static std::string ProcessIncludes(const std::string &input);

        static std::string ProcessShader(const std::string &file);

    public:
        explicit ShaderService(const std::string &shadersDirectory, bool isDebugMode, VulkanContext &vulkanContext) : shadersDirectory(
            shadersDirectory), isDebugMode(isDebugMode), vulkanContext(vulkanContext){}

        VkShaderModule createShaderModule(const std::string &pFilename);
    };
} // Metal

#endif
