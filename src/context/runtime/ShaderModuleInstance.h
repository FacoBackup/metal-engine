#ifndef SHADERRESOURCE_H
#define SHADERRESOURCE_H
#include <vector>
#include <glslang/Include/glslang_c_interface.h>

#include "RuntimeResource.h"
#include "vulkan/vulkan.h"

namespace Metal {
    struct ShaderModuleInstance final : RuntimeResource {
        std::vector<uint32_t> SPIRV;
        VkShaderModule vkShaderModule = VK_NULL_HANDLE;

        void initialize(glslang_program_t *program);
    };
} // Metal

#endif //SHADERRESOURCE_H
