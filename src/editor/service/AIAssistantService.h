#ifndef aiAssistantService_H
#define aiAssistantService_H

#include "common/IService.h"
#include "common/IInit.h"
#include "../enum/AIModel.h"
#include <string>
#include <nlohmann/json.hpp>
#include <thread>
#include <mutex>

namespace Metal {
    struct EditorRepository;
    class DirectoryService;
    struct AIAssistantRepository;
    class HttpService;
    class IToolProvider;
    class IAIResponseStrategy;
    class NotificationService;
    struct Chat;

    class AIAssistantService final : public IService, public IInit {
        EditorRepository *editorRepository = nullptr;
        DirectoryService *directoryService = nullptr;
        AIAssistantRepository *aiAssistantRepository = nullptr;
        HttpService *httpService = nullptr;
        NotificationService *notificationService = nullptr;
        std::vector<IToolProvider *> toolProviders;
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

        void sendFollowupRequest(const std::string &chatId, AIModel model, int messageIndex);

        void regenerateMessage(const std::string &chatId, int messageIndex, AIModel model);

        void editMessage(const std::string &chatId, int messageIndex, const std::string &newContent, AIModel model);

        void deleteMessagesFrom(const std::string &chatId, int messageIndex) const;

        std::string deleteCurrentChat(const std::string &chatId);

        std::string executeTool(const std::string &toolKey, const nlohmann::json &arguments) const;

        std::string getTimeStr();

    private:
        nlohmann::json buildMessages(const std::shared_ptr<Chat> &chat);

        nlohmann::json buildTools();

        std::string buildFullSystemPrompt(const std::shared_ptr<Chat> &chat);

        void processAIResponse(const std::string &chatId, int messageIndex, AIModel model, const std::string &response);

        std::string getApiKey(AIModel model);
    };
}

#endif
