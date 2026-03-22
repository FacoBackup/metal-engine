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
            std::string prompt = "Tool: " + key + "\n";
            prompt += "Description: " + description + "\n";
            prompt += "Expected JSON parameters:\n{\n";
            for (size_t i = 0; i < params.size(); ++i) {
                prompt += "  \"" + params[i].name + "\": (" + params[i].getTypeName() + ") // " + params[i].description;
                if (i < params.size() - 1) prompt += ",";
                prompt += "\n";
            }
            prompt += "}\n";
            return prompt;
        }
    };
}

#endif
