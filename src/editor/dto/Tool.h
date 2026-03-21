#ifndef TOOL_H
#define TOOL_H

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
    };

    struct Tool {
        std::string key;
        std::string description;
        std::vector<ToolParam> params;
        std::function<std::string(const std::map<std::string, std::variant<std::string, double, bool>>&)> callback;
    };
}

#endif
