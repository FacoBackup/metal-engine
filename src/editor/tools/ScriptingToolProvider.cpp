#include "ScriptingToolProvider.h"
#include "engine/service/LuaService.h"
#include "../../core/DirectoryService.h"
#include "../service/FilesService.h"
#include <fstream>
#include <filesystem>
#include <sstream>

#include "common/FileExtensions.h"

namespace Metal {
    void ScriptingToolProvider::registerTools() {
        // Tool: create_script
        registerTool("create_script", "Creates a new lua script file and writes the content to it.",
                     {
                         {"name", "The name of the script file (e.g., 'myscript.lua')", ToolParamType::STRING},
                         {"content", "The lua code content for the script.", ToolParamType::STRING}
                     },
                     [this](const nlohmann::json &params) { return createScript(params); });

        // Tool: execute_script
        registerTool("execute_script", "Executes a lua script file from the root directory.",
                     {
                         {"name", "The name of the script file to execute", ToolParamType::STRING}
                     },
                     [this](const nlohmann::json &params) { return executeScript(params); });

        // Tool: list_scripts
        registerTool("list_scripts", "Lists all available lua script files in the root directory.",
                     {},
                     [this](const nlohmann::json &params) { return listScripts(params); });
    }

    std::string ScriptingToolProvider::createScript(const nlohmann::json &params) const {
        if (!params.contains("name") || !params.contains("content"))
            return std::string("Error: Missing 'name' or 'content' parameter");

        const std::string fileName = params.at("name").get<std::string>();
        const std::string content = params.at("content").get<std::string>();

        const std::string filePath = filesService->writeRootFile(fileName, content);
        if (filePath.empty()) {
            return std::string("Error: Could not create script file. Ensure root directory is set.");
        }

        return std::string("Script created successfully at: ") + filePath;
    }

    std::string ScriptingToolProvider::executeScript(const nlohmann::json &params) const {
        if (!params.contains("name"))
            return std::string("Error: Missing 'name' parameter");

        const std::string fileName = params.at("name").get<std::string>();

        const std::filesystem::path rootPath(directoryService->getRootDirectory());

        if (const std::filesystem::path filePath = rootPath / fileName; luaService->executeFile(filePath.string())) {
            return "Script executed successfully.";
        }
        return "Error: Script execution failed.";
    }

    std::string ScriptingToolProvider::listScripts(const nlohmann::json &) const {
        std::vector<std::string> scripts = filesService->listFilesWithExtension(Metal::FileExtensions::lua->extension);

        if (scripts.empty()) {
            return "No lua scripts found or root directory not set.";
        }

        std::string result = "";
        for (const auto &script: scripts) {
            result += "- " + script + "\n";
        }

        return result;
    }
}
