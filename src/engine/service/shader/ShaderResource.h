#ifndef SHADERRESOURCE_H
#define SHADERRESOURCE_H
#include <vector>
#include <glslang/Include/glslang_c_interface.h>

#include "vulkan/vulkan.h"

namespace Metal {
    struct ShaderResource {
        std::vector<uint32_t> SPIRV;
        VkShaderModule shaderModule = nullptr;

        void initialize(glslang_program_t *program);
    };
} // Metal

#endif //SHADERRESOURCE_H
