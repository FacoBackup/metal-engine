#include "AIAssistantService.h"
#include "HttpService.h"
#include "../repository/EditorRepository.h"
#include "../repository/AIAssistantRepository.h"
#include "../../core/DirectoryService.h"
#include "../../common/LoggerUtil.h"
#include "../../common/FilesUtil.h"
#include "../../ApplicationContext.h"
#include "../dto/IToolProvider.h"
#include <nlohmann/json.hpp>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace Metal {
    void AIAssistantService::onInitialize() {
        if (directoryService) {
            try {
                const std::string path = directoryService->getRootDirectory() + "/resources/engine-system-prompt.md";
                FilesUtil::ReadFile(path.c_str(), systemPrompt);
                LOG_INFO("AIAssistantService: Loaded system prompt from " + path);
            } catch (const std::exception &e) {
                LOG_ERROR("AIAssistantService: Failed to load system prompt: " + std::string(e.what()));
            }
        }

        LOG_INFO("AIAssistantService initialized");
    }

    void AIAssistantService::sendRequest(const std::string &chatId, const std::string &message) {
        if (!editorRepository || !httpService || !aiAssistantRepository) {
            LOG_ERROR("Required components not available in AIAssistantService");
            return;
        }

        std::shared_ptr<Chat> currentChat = aiAssistantRepository->findChatById(chatId);

        if (!currentChat) {
            // This should not happen if chatId is set correctly, but handle it
            LOG_WARN("Current chat not found in repository, creating new one locally");
            currentChat = aiAssistantRepository->createNewChat();
        }

        auto getTimeStr = []() {
            auto t = std::time(nullptr);
            auto tm = *std::localtime(&t);
            std::ostringstream oss;
            oss << std::put_time(&tm, "%H:%M:%S");
            return oss.str();
        };

        // Add user message
        currentChat->messages.push_back({"user", message, getTimeStr()});

        // Store chat after the first message if it wasn't stored before
        if (currentChat->messages.size() == 1 && !aiAssistantRepository->findChatById(currentChat->id)) {
            aiAssistantRepository->addChat(currentChat);
        }

        LOG_INFO("Sending MCP request to: " + editorRepository->mcpModel);

        nlohmann::json body;
        body["model"] = "gpt-3.5-turbo";
        body["messages"] = nlohmann::json::array();
        
        if (!systemPrompt.empty()) {
            body["messages"].push_back({{"role", "system"}, {"content", systemPrompt}});
        }
        
        for (const auto& msg : currentChat->messages) {
            body["messages"].push_back({{"role", msg.role}, {"content", msg.content}});
        }

        std::string targetChatId = currentChat->id;
        // httpService->post(editorRepository->mcpModel + "/chat/completions", body.dump(), editorRepository->mcpToken,
        //     [this, targetChatId, getTimeStr](const std::string& response, bool success) {
        //         if (success) {
        //             try {
        //                 auto j = nlohmann::json::parse(response);
        //                 if (j.contains("choices") && !j["choices"].empty()) {
        //                     std::string content = j["choices"][0]["message"]["content"];
        //
        //                     auto chatOpt = aiAssistantRepository->findChatById(targetChatId);
        //                     if (chatOpt) {
        //                         (*chatOpt)->messages.push_back({"assistant", content, getTimeStr()});
        //                         LOG_INFO("MCP Response added to chat " + targetChatId);
        //                     }
        //                 }
        //             } catch (const std::exception& e) {
        //                 LOG_ERROR("Failed to parse MCP response: " + std::string(e.what()));
        //             }
        //         } else {
        //             LOG_ERROR("MCP Request failed: " + response);
        //         }
        //     });
    }

    std::string AIAssistantService::createNewChat() {
        if (!aiAssistantRepository) return "";
        auto newChat = aiAssistantRepository->createNewChat();
        return newChat->id;
        // Do not add to repository yet, wait for first message
    }

    std::string AIAssistantService::deleteCurrentChat(const std::string &chatId) {
        if (!aiAssistantRepository) return "";
        aiAssistantRepository->deleteChat(chatId);
        return createNewChat();
    }
}
