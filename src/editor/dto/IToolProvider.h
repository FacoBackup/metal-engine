#ifndef ITOOLPROVIDER_H
#define ITOOLPROVIDER_H

#include "Tool.h"
#include <vector>
#include <string>

namespace Metal {
    class IToolProvider {
        std::vector<Tool> tools;
        bool toolsRegistered = false;

    protected:
        virtual void registerTools() = 0;

        void registerTool(const std::string& key, const std::string& description, 
                         const std::vector<ToolParam>& params, 
                         const std::function<std::string(const std::map<std::string, std::variant<std::string, double, bool>>&)>& callback) {
            tools.push_back({key, description, params, callback});
        }

    public:
        virtual ~IToolProvider() = default;

        const std::vector<Tool>& getTools() {
            if (!toolsRegistered) {
                registerTools();
                toolsRegistered = true;
            }
            return tools;
        }
    };
}

#endif
