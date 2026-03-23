#include "ScriptingToolProvider.h"
#include "engine/service/LuaService.h"
#include "../../core/DirectoryService.h"
#include "../service/FilesService.h"
#include <fstream>
#include <filesystem>
#include <sstream>

namespace Metal {
    void ScriptingToolProvider::registerTools() {
        // Tool: create_script
        registerTool("create_script", "Creates a new lua script file and writes the content to it.", 
            {
                {"name", "The name of the script file (e.g., 'myscript.lua')", ToolParamType::STRING},
                {"content", "The lua code content for the script.", ToolParamType::STRING}
            }, 
            [this](const nlohmann::json& params) {
                if (!params.contains("name") || !params.contains("content"))
                    return std::string("Error: Missing 'name' or 'content' parameter");
                
                std::string fileName = params.at("name").get<std::string>();
                std::string content = params.at("content").get<std::string>();
                
                std::string filePath = filesService->writeRootFile(fileName, content);
                if (filePath.empty()) {
                    return std::string("Error: Could not create script file. Ensure root directory is set.");
                }
                
                return std::string("Script created successfully at: ") + filePath;
            });

        // Tool: execute_script
        registerTool("execute_script", "Executes a lua script file from the root directory.", 
            {
                {"name", "The name of the script file to execute", ToolParamType::STRING}
            }, 
            [this](const nlohmann::json& params) {
                if (!params.contains("name"))
                    return std::string("Error: Missing 'name' parameter");
                
                std::string fileName = params.at("name").get<std::string>();
                
                std::filesystem::path rootPath(directoryService->getRootDirectory());
                std::filesystem::path filePath = rootPath / fileName;
                
                if (luaService->executeFile(filePath.string())) {
                    return std::string("Script executed successfully.");
                } else {
                    return std::string("Error: Script execution failed.");
                }
            });

        // Tool: list_scripts
        registerTool("list_scripts", "Lists all available lua script files in the root directory.", 
            {}, 
            [this](const nlohmann::json& params) {
                std::vector<std::string> scripts = filesService->listScripts();
                
                if (scripts.empty()) {
                    return std::string("No lua scripts found or root directory not set.");
                }
                
                std::string result = "Available scripts:\n";
                for (const auto& script : scripts) {
                    result += "- " + script + "\n";
                }
                
                return result;
            });
    }
}
