#ifndef aiAssistantService_H
#define aiAssistantService_H

#include "../../common/IService.h"
#include "../../common/IInit.h"
#include "../enum/AIModel.h"
#include <string>
#include <nlohmann/json.hpp>

namespace Metal {
    struct EditorRepository;
    struct DirectoryService;
    struct AIAssistantRepository;
    class HttpService;
    class IToolProvider;
    class NotificationService;
    struct Chat;

    class AIAssistantService final : public IService, public IInit {
        EditorRepository *editorRepository = nullptr;
        DirectoryService *directoryService = nullptr;
        AIAssistantRepository *aiAssistantRepository = nullptr;
        HttpService *httpService = nullptr;
        NotificationService *notificationService = nullptr;
        std::vector<IToolProvider*> toolProviders;
        std::string systemPrompt;

    public:
        AIAssistantService() = default;

        ~AIAssistantService() override = default;

        std::vector<Dependency> getDependencies() override {
            return {
                {"EditorRepository", &editorRepository},
                {"DirectoryService", &directoryService},
                {"AIAssistantRepository", &aiAssistantRepository},
                {"HttpService", &httpService},
                {"NotificationService", &notificationService}
            };
        }

        void onInitialize() override;

        void sendRequest(const std::string &chatId, const std::string &message, AIModel model);

        std::string createNewChat();

        std::string deleteCurrentChat(const std::string &chatId);

    private:
        nlohmann::json buildMessages(const std::shared_ptr<Chat>& chat);

        std::string buildFullSystemPrompt();

        void processAIResponse(const std::string& chatId, AIModel model, const std::string& response);

        std::string executeTool(const std::string& toolKey, const nlohmann::json& arguments);
        
        std::string getApiKey(AIModel model);

        std::string getTimeStr();
    };
}

#endif
