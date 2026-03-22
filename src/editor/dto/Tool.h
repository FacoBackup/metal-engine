#ifndef TOOL_H
#define TOOL_H

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <functional>
#include <variant>
#include <map>

namespace Metal {
    enum class ToolParamType {
        STRING,
        NUMBER,
        BOOLEAN
    };

    struct ToolParam {
        std::string name;
        std::string description;
        ToolParamType type;

        std::string getTypeName() const {
            switch (type) {
                case ToolParamType::STRING: return "string";
                case ToolParamType::NUMBER: return "number";
                case ToolParamType::BOOLEAN: return "boolean";
                default: return "unknown";
            }
        }
    };

    struct Tool {
        std::string key;
        std::string description;
        std::vector<ToolParam> params;
        std::function<std::string(const nlohmann::json&)> callback;

        std::string getSystemPrompt() const {
            nlohmann::json toolJson;
            toolJson["name"] = key;
            toolJson["description"] = description;

            nlohmann::json properties = nlohmann::json::object();
            std::vector<std::string> required;

            for (const auto& param : params) {
                nlohmann::json paramJson;
                paramJson["type"] = param.getTypeName();
                paramJson["description"] = param.description;
                properties[param.name] = paramJson;
                required.push_back(param.name);
            }

            nlohmann::json inputSchema;
            inputSchema["type"] = "object";
            inputSchema["properties"] = properties;
            if (!required.empty()) {
                inputSchema["required"] = required;
            }

            toolJson["inputSchema"] = inputSchema;
            return toolJson.dump(2);
        }
    };
}

#endif
