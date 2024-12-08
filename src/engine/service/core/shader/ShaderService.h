#ifndef SHADERSERVICE_H
#define SHADERSERVICE_H

#include <glslang/Public/ShaderLang.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <glslang/Include/glslang_c_shader_types.h>
#include <vulkan/vulkan_core.h>

#include "ShaderModuleInstance.h"
#include "../../AbstractResourceService.h"
#include "../../../../common/interface/AbstractRuntimeComponent.h"

namespace Metal {
    class ShaderService final : public AbstractResourceService {
        bool compileShader(glslang_stage_t stage, const char *pShaderCode, ShaderModuleInstance *shaderModule) const;

        static glslang_stage_t ShaderStageFromFilename(const char *pFilename);

    public:
        explicit ShaderService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }

        ShaderModuleInstance *createShaderModule(const char *pFilename) const;
    };
} // Metal

#endif
