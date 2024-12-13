#include "ShaderService.h"

#include "../runtime/ShaderModuleInstance.h"
#include "../../common/util/VulkanUtils.h"
#include "../../common/util/Util.h"
#include "../ApplicationContext.h"
#include "../../common/util/files/FilesUtil.h"
#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/resource_limits_c.h"

namespace Metal {
    bool ShaderService::compileShader(const glslang_stage_t stage, const char *pShaderCode,
                                      ShaderModuleInstance *shaderModule) const {
        const glslang_input_t input = {
            .language = GLSLANG_SOURCE_GLSL,
            .stage = stage,
            .client = GLSLANG_CLIENT_VULKAN,
            .client_version = GLSLANG_TARGET_VULKAN_1_3,
            .target_language = GLSLANG_TARGET_SPV,
            .target_language_version = GLSLANG_TARGET_SPV_1_6,
            .code = pShaderCode,
            .default_version = 460,
            .default_profile = GLSLANG_NO_PROFILE,
            .force_default_version_and_profile = false,
            .forward_compatible = false,
            .messages = static_cast<glslang_messages_t>(
                GLSLANG_MSG_DEFAULT_BIT | GLSLANG_MSG_VULKAN_RULES_BIT | GLSLANG_MSG_SPV_RULES_BIT),
            .resource = glslang_default_resource()
        };

        glslang_shader_t *shader = glslang_shader_create(&input);

        if (!glslang_shader_preprocess(shader, &input)) {
            throw std::runtime_error("Failed to process shader");
        }
        if (!glslang_shader_parse(shader, &input)) {
            throw std::runtime_error("Failed to parse shader");
        }

        glslang_program_t *program = glslang_program_create();
        glslang_program_add_shader(program, shader);
        if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT)) {
            throw std::runtime_error("Failed to link program");
        }

        glslang_program_SPIRV_generate(program, stage);

        shaderModule->initialize(program);

        if (const char *spirv_messages = glslang_program_SPIRV_get_messages(program)) {
            printf("SPIR-V message: '%s'", spirv_messages);
        }

        VkShaderModuleCreateInfo shaderCreateInfo{};
        shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderCreateInfo.codeSize = shaderModule->SPIRV.size() * sizeof(uint32_t);
        shaderCreateInfo.pCode = static_cast<const uint32_t *>(shaderModule->SPIRV.data());

        VulkanUtils::CheckVKResult(vkCreateShaderModule(context.getVulkanContext().device.device, &shaderCreateInfo,
                                                        nullptr,
                                                        &shaderModule->vkShaderModule));
        glslang_program_delete(program);
        glslang_shader_delete(shader);

        return !shaderModule->SPIRV.empty();
    }

    glslang_stage_t ShaderService::ShaderStageFromFilename(const char *pFilename) {
        const std::string s(pFilename);

        if (s.ends_with(".vert")) {
            return GLSLANG_STAGE_VERTEX;
        }

        if (s.ends_with(".frag")) {
            return GLSLANG_STAGE_FRAGMENT;
        }

        if (s.ends_with(".geom")) {
            return GLSLANG_STAGE_GEOMETRY;
        }

        if (s.ends_with(".comp")) {
            return GLSLANG_STAGE_COMPUTE;
        }

        if (s.ends_with(".tesc")) {
            return GLSLANG_STAGE_TESSCONTROL;
        }

        if (s.ends_with(".tese")) {
            return GLSLANG_STAGE_TESSEVALUATION;
        }
        throw std::runtime_error("Unknown shader stage in file");
    }

    ShaderModuleInstance *ShaderService::createShaderModule(const char *pFilename) {
        const std::string basePath = context.getRootDirectory() + "/shaders/";

        FilesUtil::MkDir(basePath);
        std::string source;
        FilesUtil::ReadFile(pFilename, source);

        const glslang_stage_t shaderStage = ShaderStageFromFilename(pFilename);

        glslang_initialize_process();
        auto *shader = new ShaderModuleInstance;
        if (compileShader(shaderStage, source.c_str(), shader)) {
            const std::string part(pFilename);
            const std::string BinaryFilename = basePath + part.substr(
                                                   part.find_last_of('/') + 1, part.size()) + ".spv";
            FilesUtil::WriteBinaryFile(BinaryFilename.c_str(), shader->SPIRV.data(),
                                       shader->SPIRV.size() * sizeof(uint32_t));
            registerResource(shader);
        } else {
            delete shader;
            shader = nullptr;
        }
        glslang_finalize_process();

        return shader;
    }
}
