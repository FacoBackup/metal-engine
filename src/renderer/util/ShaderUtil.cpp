#include "ShaderUtil.h"

#include <regex>
#include "../../common/Logger.h"
#include "../data/ShaderModule.h"
#include "VulkanUtils.h"
#include "../../common/Util.h"
#include "../../MetalContextProvider.h"
#include "../../common/FilesUtil.h"
#include "../../common/enum/LightVolumeType.h"
#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/resource_limits_c.h"
#define BASE_PATH "../resources/shaders/"

namespace Metal
{
    void ShaderUtil::CheckShaderCompilation(glslang_shader_t* shader)
    {
        const char* infoLog = glslang_shader_get_info_log(shader);
        const char* debugLog = glslang_shader_get_info_debug_log(shader);

        if (infoLog)
        {
            printf("Shader Info Log:\n%s\n", infoLog);
        }
        if (debugLog)
        {
            printf("Shader Debug Log:\n%s\n", debugLog);
        }
    }

    bool ShaderUtil::CompileShader(glslang_stage_t stage, const char* pShaderCode,
                                   ShaderModule* shaderModule)
    {
        const glslang_input_t input = {
            .language = GLSLANG_SOURCE_GLSL,
            .stage = stage,
            .client = GLSLANG_CLIENT_VULKAN,
            .client_version = GLSLANG_TARGET_VULKAN_1_1,
            .target_language = GLSLANG_TARGET_SPV,
            .target_language_version = GLSLANG_TARGET_SPV_1_6,
            .code = pShaderCode,
            .default_version = 460,
            .default_profile = GLSLANG_NO_PROFILE,
            .force_default_version_and_profile = false,
            .forward_compatible = false,
            .messages = GLSLANG_MSG_DEFAULT_BIT,
            .resource = glslang_default_resource()
        };

        glslang_shader_t* shader = glslang_shader_create(&input);

        if (!glslang_shader_preprocess(shader, &input))
        {
            CheckShaderCompilation(shader);
            throw std::runtime_error("Failed to process shader");
        }
        if (!glslang_shader_parse(shader, &input))
        {
            CheckShaderCompilation(shader);
            throw std::runtime_error("Failed to parse shader");
        }

        glslang_program_t* program = glslang_program_create();
        glslang_program_add_shader(program, shader);
        if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT))
        {
            throw std::runtime_error("Failed to link program");
        }

        glslang_program_SPIRV_generate(program, stage);

        shaderModule->initialize(program);

        if (const char* spirv_messages = glslang_program_SPIRV_get_messages(program))
        {
            WARN("SPIR-V message: {}", spirv_messages);
        }

        VkShaderModuleCreateInfo shaderCreateInfo{};
        shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderCreateInfo.codeSize = shaderModule->SPIRV.size() * sizeof(unsigned int);
        shaderCreateInfo.pCode = static_cast<const unsigned int*>(shaderModule->SPIRV.data());

        VulkanUtils::CheckVKResult(vkCreateShaderModule(SINGLETONS.vulkanContext.device.device,
                                                        &shaderCreateInfo,
                                                        nullptr,
                                                        &shaderModule->vkShaderModule));
        glslang_program_delete(program);
        glslang_shader_delete(shader);

        return !shaderModule->SPIRV.empty();
    }

    glslang_stage_t ShaderUtil::ShaderStageFromFilename(const char* pFilename)
    {
        const std::string s(pFilename);

        if (s.ends_with(".vert"))
        {
            return GLSLANG_STAGE_VERTEX;
        }

        if (s.ends_with(".frag"))
        {
            return GLSLANG_STAGE_FRAGMENT;
        }

        if (s.ends_with(".geom"))
        {
            return GLSLANG_STAGE_GEOMETRY;
        }

        if (s.ends_with(".comp"))
        {
            return GLSLANG_STAGE_COMPUTE;
        }

        if (s.ends_with(".tesc"))
        {
            return GLSLANG_STAGE_TESSCONTROL;
        }

        if (s.ends_with(".tese"))
        {
            return GLSLANG_STAGE_TESSEVALUATION;
        }
        throw std::runtime_error("Unknown shader stage in file");
    }

    std::string ShaderUtil::ProcessIncludes(const std::string& input)
    {
        std::string result = input;
        std::regex includePattern(R"(#include\s+"(.+))");
        std::smatch match;

        while (std::regex_search(result, match, includePattern))
        {
            std::string includeFile = match[1].str();
            if (includeFile.find("./") == 0 || includeFile.find("../") == 0)
            {
                includeFile = includeFile.substr(includeFile.find_first_of("/\\") + 1, includeFile.size());
                includeFile.erase(std::ranges::remove(includeFile, '"').begin(), includeFile.end());
            }
            try
            {
                std::string source;
                FilesUtil::ReadFile((BASE_PATH + includeFile).c_str(), source);
                result.replace(match.position(0), match.length(0), source);
            }
            catch (const std::exception& e)
            {
                ERROR("Error loading included shader: {}", e.what());
                return "";
            }
        }
        if (result.find("#include") != std::string::npos)
        {
            result = ProcessIncludes(result);
        }
        return result;
    }

    std::string ShaderUtil::ProcessShader(const std::string& file)
    {
        std::string source;
        FilesUtil::ReadFile(file.c_str(), source);
        return ProcessIncludes(source);
    }

    VkShaderModule ShaderUtil::CreateShaderModule(const std::string& pFilename)
    {
        const std::string basePath = ContextProvider::Get().getShadersDirectory();
        std::string source = ProcessShader(BASE_PATH + pFilename);
        if (ContextProvider::Get().isDebugMode())
        {
            source = "#define DEBUG\n" + source;
        }
        for (auto& entry : LightVolumeTypes::getEntries())
        {
            source = "#define " + entry.first + " " + std::to_string(entry.second) + "\n" + source;
        }
        source = "#define MAX_LIGHT_VOLUMES " + std::to_string(MAX_LIGHT_VOLUMES) + std::string("\n") + source;
        source = "#define PI_2 6.28318530718\n" + source;
        source = "#define PI 3.14159265\n" + source;

        source = "#define MAX_RT_TRIANGLES " + std::to_string(MAX_RT_TRIANGLES) + std::string("\n") + source;
        source = "#define MAX_RT_BLAS " + std::to_string(MAX_RT_BLAS) + std::string("\n") + source;
        source = "#define MAX_RT_TLAS " + std::to_string(MAX_RT_TLAS) + std::string("\n") + source;
        source = "#define MAX_MATERIALS " + std::to_string(MAX_MATERIALS) + std::string("\n") + source;

        const glslang_stage_t shaderStage = ShaderStageFromFilename(pFilename.c_str());

        glslang_initialize_process();
        ShaderModule shader{};
        if (CompileShader(shaderStage, source.c_str(), &shader))
        {
            const std::string part(BASE_PATH + pFilename);
            const std::string BinaryFilename = basePath + part.substr(
                part.find_last_of('/') + 1, part.size()) + ".spv";
            FilesUtil::WriteBinaryFile(BinaryFilename.c_str(), shader.SPIRV.data(),
                                       shader.SPIRV.size() * sizeof(unsigned int));
        }
        glslang_finalize_process();
        LOG("Success!");
        return shader.vkShaderModule;
    }
}
