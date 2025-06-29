#include "ShaderModule.h"

#include "../VulkanContext.h"

namespace Metal {
    void ShaderModule::initialize(glslang_program_t *program) {
        size_t program_size = glslang_program_SPIRV_get_size(program);
        SPIRV.resize(program_size);
        glslang_program_SPIRV_get(program, SPIRV.data());
    }
}
