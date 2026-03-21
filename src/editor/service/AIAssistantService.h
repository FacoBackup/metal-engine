#ifndef aiAssistantService_H
#define aiAssistantService_H

#include "../../common/IService.h"
#include "../../common/IInit.h"
#include <string>

namespace Metal {
    struct EditorRepository;
    struct DirectoryService;
    struct AIAssistantRepository;
    class HttpService;

    class AIAssistantService final : public IService, public IInit {
        EditorRepository *editorRepository = nullptr;
        DirectoryService *directoryService = nullptr;
        AIAssistantRepository *aiAssistantRepository = nullptr;
        HttpService *httpService = nullptr;
        std::string systemPrompt;

    public:
        AIAssistantService() = default;

        ~AIAssistantService() override = default;

        std::vector<Dependency> getDependencies() override {
            return {
                {"EditorRepository", &editorRepository},
                {"DirectoryService", &directoryService},
                {"AIAssistantRepository", &aiAssistantRepository},
                {"HttpService", &httpService}
            };
        }

        void onInitialize() override;

        void sendRequest(const std::string &chatId, const std::string &message);

        std::string createNewChat();

        std::string deleteCurrentChat(const std::string &chatId);
    };
}

#endif
