#ifndef SHADERSERVICE_H
#define SHADERSERVICE_H
#include <string>
#include "../dto/ShaderModule.h"
#include "../../common/IService.h"

namespace Metal {
    class VulkanContext;

    class DirectoryService;

    class ShaderService final : public IService {
        VulkanContext *vulkanContext = nullptr;
        DirectoryService *directoryService = nullptr;

        static void CheckShaderCompilation(glslang_shader_t *shader);

        bool CompileShader(glslang_stage_t stage, const char *pShaderCode,
                           ShaderModule *shaderModule) const;

        static glslang_stage_t ShaderStageFromFilename(const char *pFilename);

        std::string ProcessIncludes(const std::string &input);

        std::string ProcessShader(const std::string &file);

    public:
        ShaderService() = default;

        std::vector<Dependency> getDependencies() override {
            return {
                {"VulkanContext", &vulkanContext},
                {"DirectoryService", &directoryService}
            };
        }

        VkShaderModule createShaderModule(const std::string &pFilename);
    };
} // Metal

#endif
