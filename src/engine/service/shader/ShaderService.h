#ifndef SHADERSERVICE_H
#define SHADERSERVICE_H

#include <glslang/Public/ShaderLang.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <glslang/Include/glslang_c_shader_types.h>
#include <vulkan/vulkan_core.h>

#include "ShaderResource.h"
#include "../../../common/runtime/AbstractRuntimeComponent.h"

namespace Metal {
    class ShaderService final : public AbstractRuntimeComponent {
        bool compileShader(glslang_stage_t Stage, const char *pShaderCode, ShaderResource &shaderModule) const;

        static glslang_stage_t ShaderStageFromFilename(const char *pFilename);

    public:
        explicit ShaderService(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        VkShaderModule createShaderModule(const char *pFilename) const;
    };
} // Metal

#endif
