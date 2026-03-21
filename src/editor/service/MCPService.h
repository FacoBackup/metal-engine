#ifndef MCPSERVICE_H
#define MCPSERVICE_H

#include "../../common/IService.h"
#include "../../common/IInit.h"
#include <string>

namespace Metal {
    struct EditorRepository;

    class MCPService final : public IService, public IInit {
        EditorRepository *editorRepository = nullptr;

    public:
        MCPService() = default;
        ~MCPService() override = default;

        std::vector<Dependency> getDependencies() override {
            return {
                {"EditorRepository", &editorRepository}
            };
        }

        void onInitialize() override;

        void sendRequest(const std::string& message);
    };
}

#endif
