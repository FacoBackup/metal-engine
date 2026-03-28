#ifndef SCRIPTINGTOOLS_H
#define SCRIPTINGTOOLS_H

#include "../dto/IToolProvider.h"

namespace Metal {
    class LuaService;
    class DirectoryService;
    class FilesService;

    class ScriptingToolProvider final : public IToolProvider {
        LuaService *luaService = nullptr;
        DirectoryService *directoryService = nullptr;
        FilesService *filesService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"LuaService", &luaService},
                {"DirectoryService", &directoryService},
                {"FilesService", &filesService}
            };
        }

    protected:
        void registerTools() override;

    private:
        std::string createScript(const nlohmann::json &params) const;
        std::string executeScript(const nlohmann::json &params) const;
        std::string listScripts(const nlohmann::json &params) const;
    };
}

#endif
