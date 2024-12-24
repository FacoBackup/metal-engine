#include "ShaderUtil.h"

#include <regex>

#include "../service/pipeline/ShaderModule.h"
#include "../util/VulkanUtils.h"
#include "../util/Util.h"
#include "../context/ApplicationContext.h"
#include "FilesUtil.h"
#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/resource_limits_c.h"
#define BASE_PATH "../resources/shaders/"

namespace Metal {
    void ShaderUtil::CheckShaderCompilation(glslang_shader_t *shader) {
        const char *infoLog = glslang_shader_get_info_log(shader);
        const char *debugLog = glslang_shader_get_info_debug_log(shader);

        if (infoLog) {
            printf("Shader Info Log:\n%s\n", infoLog);
        }
        if (debugLog) {
            printf("Shader Debug Log:\n%s\n", debugLog);
        }
    }

    bool ShaderUtil::CompileShader(const VulkanContext &context, glslang_stage_t stage, const char *pShaderCode,
                                   ShaderModule *shaderModule) {
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
            CheckShaderCompilation(shader);
            throw std::runtime_error("Failed to process shader");
        }
        if (!glslang_shader_parse(shader, &input)) {
            CheckShaderCompilation(shader);
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

        VulkanUtils::CheckVKResult(vkCreateShaderModule(context.device.device, &shaderCreateInfo,
                                                        nullptr,
                                                        &shaderModule->vkShaderModule));
        glslang_program_delete(program);
        glslang_shader_delete(shader);

        return !shaderModule->SPIRV.empty();
    }

    glslang_stage_t ShaderUtil::ShaderStageFromFilename(const char *pFilename) {
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

    std::string ShaderUtil::ProcessIncludes(const std::string &input) {
        std::string result = input;
        std::regex includePattern(R"(#include\s+"(.+))");
        std::smatch match;

        while (std::regex_search(result, match, includePattern)) {
            std::string includeFile = match[1].str();
            if (includeFile.find("./") == 0 || includeFile.find("../") == 0) {
                includeFile = includeFile.substr(includeFile.find_first_of("/\\") + 1, includeFile.size());
                includeFile.erase(std::ranges::remove(includeFile, '"').begin(), includeFile.end());
            }
            try {
                std::string source;
                FilesUtil::ReadFile((BASE_PATH + includeFile).c_str(), source);
                result.replace(match.position(0), match.length(0), source);
            } catch (const std::exception &e) {
                std::cerr << "Error loading included shader: " << e.what() << std::endl;
                return "";
            }
        }
        if (result.find("#include") != std::string::npos) {
            result = ProcessIncludes(result);
        }
        return result;
    }

    std::string ShaderUtil::ProcessShader(const std::string &file) {
        std::string source;
        FilesUtil::ReadFile(file.c_str(), source);
        return ProcessIncludes(source);
    }

    VkShaderModule ShaderUtil::CreateShaderModule(const ApplicationContext &context, const std::string &pFilename, bool debugMode) {
        const std::string basePath = context.getShadersDirectory();
        std::string source = ProcessShader(BASE_PATH + pFilename);
        if (debugMode) {
            source = "#define DEBUG\n" + source;
        }
        const glslang_stage_t shaderStage = ShaderStageFromFilename(pFilename.c_str());

        glslang_initialize_process();
        ShaderModule shader{};
        if (CompileShader(context.vulkanContext, shaderStage, source.c_str(), &shader)) {
            const std::string part(BASE_PATH + pFilename);
            const std::string BinaryFilename = basePath + part.substr(
                                                   part.find_last_of('/') + 1, part.size()) + ".spv";
            FilesUtil::WriteBinaryFile(BinaryFilename.c_str(), shader.SPIRV.data(),
                                       shader.SPIRV.size() * sizeof(uint32_t));
        }
        glslang_finalize_process();
        return shader.vkShaderModule;
    }
}
