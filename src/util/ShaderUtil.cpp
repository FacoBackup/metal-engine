#include "ShaderUtil.h"

#include <regex>
#include <fstream>

#include "../service/pipeline/ShaderModule.h"
#include "../util/VulkanUtils.h"
#include "../util/Util.h"
#include "../context/ApplicationContext.h"
#include "FilesUtil.h"
#include "../service/log/LogService.h"
#include "../enum/LightType.h"
#include "../enum/ShadingMode.h"
#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/resource_limits_c.h"
#define BASE_PATH "../resources/shaders/"

namespace Metal {
    void ShaderUtil::CheckShaderCompilation(glslang_shader_t *shader) {
        const char *infoLog = glslang_shader_get_info_log(shader);
        const char *debugLog = glslang_shader_get_info_debug_log(shader);
        const char *shaderCode = glslang_shader_get_preprocessed_code(shader);

        if (infoLog && strlen(infoLog) > 0) {
            LOG_ERROR("Shader Info Log: " + std::string(infoLog) + "\n\n" + std::string(shaderCode));
        }
        if (debugLog && strlen(debugLog) > 0) {
            LOG_DEBUG("Shader Debug Log: " + std::string(debugLog));
        }
    }

    bool ShaderUtil::CompileShader(glslang_stage_t stage, const char *pShaderCode,
                                   ShaderModule *shaderModule) {
        const glslang_input_t input = {
            .language = GLSLANG_SOURCE_GLSL,
            .stage = stage,
            .client = GLSLANG_CLIENT_VULKAN,
            .client_version = GLSLANG_TARGET_VULKAN_1_2,
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
            if (strlen(spirv_messages) > 0) {
                LOG_DEBUG("SPIR-V message: " + std::string(spirv_messages));
            }
        }

        VkShaderModuleCreateInfo shaderCreateInfo{};
        shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderCreateInfo.codeSize = shaderModule->SPIRV.size() * sizeof(unsigned int);
        shaderCreateInfo.pCode = static_cast<const unsigned int *>(shaderModule->SPIRV.data());

        VulkanUtils::CheckVKResult(vkCreateShaderModule(CTX.vulkanContext.device.device, &shaderCreateInfo,
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

        if (s.ends_with(".rgen")) {
            return GLSLANG_STAGE_RAYGEN;
        }

        if (s.ends_with(".rmiss")) {
            return GLSLANG_STAGE_MISS;
        }

        if (s.ends_with(".rchit")) {
            return GLSLANG_STAGE_CLOSESTHIT;
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
                LOG_ERROR("Error loading included shader: " + std::string(e.what()));
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

    VkShaderModule ShaderUtil::CreateShaderModule(const std::string &pFilename) {
        const std::string basePath = CTX.getShadersDirectory();
        std::string source = ProcessShader(BASE_PATH + pFilename);
        if (CTX.isDebugMode()) {
            source = "#define DEBUG\n" + source;
        }
        for (auto &entry: ShadingModes::getShaderEntries()) {
            source = "#define " + entry.first + " " + std::to_string(entry.second) + "\n" + source;
        }
        for (auto &entry: LightTypes::getEntries()) {
            source = "#define " + entry.first + " " + std::to_string(entry.second) + "\n" + source;
        }
        source = "#define PI_2 6.28318530718\n" + source;
        source = "#define PI 3.14159265\n" + source;

        const size_t sourceHash = std::hash<std::string>{}(source);
        const std::string part(BASE_PATH + pFilename);
        const std::string shaderName = part.substr(part.find_last_of('/') + 1, part.size());
        const std::string binaryFilename = basePath + shaderName + ".spv";
        const std::string hashFilename = basePath + shaderName + ".hash";

        ShaderModule shader{};

        bool needsCompilation = true;
        if (std::filesystem::exists(binaryFilename) && std::filesystem::exists(hashFilename)) {
            size_t cachedHash = 0;
            std::ifstream hashFile(hashFilename);
            if (hashFile >> cachedHash && cachedHash == sourceHash) {
                try {
                    FilesUtil::ReadBinaryFile(binaryFilename.c_str(), shader.SPIRV);

                    VkShaderModuleCreateInfo shaderCreateInfo{};
                    shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                    shaderCreateInfo.codeSize = shader.SPIRV.size() * sizeof(unsigned int);
                    shaderCreateInfo.pCode = static_cast<const unsigned int *>(shader.SPIRV.data());

                    if (vkCreateShaderModule(CTX.vulkanContext.device.device, &shaderCreateInfo,
                                             nullptr, &shader.vkShaderModule) == VK_SUCCESS) {
                        needsCompilation = false;
                        LOG_INFO("Loaded cached shader: " + shaderName);
                    }
                } catch (...) {
                    needsCompilation = true;
                }
            }
        }

        if (needsCompilation) {
            const glslang_stage_t shaderStage = ShaderStageFromFilename(pFilename.c_str());
            glslang_initialize_process();
            LOG_INFO("Compiling shader: " + shaderName);
            if (CompileShader(shaderStage, source.c_str(), &shader)) {
                FilesUtil::WriteBinaryFile(binaryFilename.c_str(), shader.SPIRV.data(),
                                           shader.SPIRV.size() * sizeof(unsigned int));
                FilesUtil::WriteFile(hashFilename.c_str(), std::to_string(sourceHash).c_str());
            }
            glslang_finalize_process();
        }

        return shader.vkShaderModule;
    }
}
