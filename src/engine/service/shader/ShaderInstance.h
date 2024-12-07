#ifndef SHADERRESOURCE_H
#define SHADERRESOURCE_H
#include <vector>
#include <glslang/Include/glslang_c_interface.h>

#include "../../repository/resource/RuntimeResource.h"
#include "vulkan/vulkan.h"

namespace Metal {
    struct ShaderInstance final : RuntimeResource {
        std::vector<uint32_t> SPIRV;
        VkShaderModule shaderModule = VK_NULL_HANDLE;

        void initialize(glslang_program_t *program);
    };
} // Metal

#endif //SHADERRESOURCE_H
