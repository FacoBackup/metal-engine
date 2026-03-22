#include "AIAssistantService.h"
#include "HttpService.h"
#include "NotificationService.h"
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
        toolProviders = ctx->getSingletons<IToolProvider>();
        LOG_INFO("AIAssistantService: Found " + std::to_string(toolProviders.size()) + " tool providers");

        try {
            FilesUtil::ReadFile("../resources/engine-system-prompt.md", systemPrompt);
        } catch (const std::exception &e) {
            LOG_ERROR("AIAssistantService: Failed to load system prompt: " + std::string(e.what()));
        }

        LOG_INFO("AIAssistantService initialized");
    }

    void AIAssistantService::sendRequest(const std::string &chatId, const std::string &message, AIModel model) {
        std::shared_ptr<Chat> currentChat = aiAssistantRepository->findChatById(chatId);

        if (!currentChat) {
            LOG_ERROR("Current chat not found in repository");
            return;
        }

        std::string apiKey = getApiKey(model);
        if (apiKey.empty()) return;

        AIModelInfo modelInfo = getAIModelInfo(model);

        // Add user message
        {
            std::lock_guard<std::mutex> lock(repositoryMutex);
            currentChat->messages.push_back({"user", message, getTimeStr()});
        }

        LOG_INFO("Sending Async MCP request to: " + modelInfo.url + " using model " + modelInfo.modelId);

        nlohmann::json body;
        body["model"] = modelInfo.modelId;
        body["messages"] = buildMessages(currentChat);
        body["stream"] = true;

        std::string targetChatId = currentChat->id;
        std::string jsonBody = body.dump();
        std::string url = modelInfo.url + "/chat/completions";

        std::thread([this, url, jsonBody, apiKey, targetChatId, model]() {
            std::string accumulatedData;
            std::string currentContent;
            httpService->postStream(url, jsonBody, apiKey,
                                    [this, targetChatId, &accumulatedData, &currentContent](
                                const char *data, size_t len) {
                                        processStreamData(targetChatId, std::string(data, len), accumulatedData,
                                                          currentContent);
                                        return true;
                                    },
                                    [this, targetChatId, model](const std::string &fullResponse, bool success) {
                                        if (success) {
                                            LOG_INFO("Async MCP Request finished for " + targetChatId);
                                            // Final processing for tool calls if any
                                            processAIResponse(targetChatId, model, fullResponse);
                                        } else {
                                            LOG_ERROR("Async MCP Request failed: " + fullResponse);
                                            if (notificationService) {
                                                notificationService->pushMessage(
                                                    "AI Request failed: " + fullResponse,
                                                    NotificationSeverities::ERROR);
                                            }
                                        }
                                    });
            directoryService->save(true);
        }).detach();
    }

    void AIAssistantService::processAIResponse(const std::string &chatId, AIModel model, const std::string &response) {
        try {
            auto j = nlohmann::json::parse(response);
            if (!j.contains("choices") || j["choices"].empty()) {
                LOG_ERROR("AI Response missing choices");
                return;
            }

            auto &message = j["choices"][0]["message"];
            std::string content = message.value("content", "");

            std::shared_ptr<Chat> chat;
            {
                std::lock_guard<std::mutex> lock(repositoryMutex);
                chat = aiAssistantRepository->findChatById(chatId);
            }
            if (!chat) return;

            int promptTokens = 0;
            int completionTokens = 0;
            int totalTokens = 0;

            {
                std::lock_guard<std::mutex> lock(repositoryMutex);
                if (j.contains("usage")) {
                    auto &usage = j["usage"];
                    promptTokens = usage.value("prompt_tokens", 0);
                    completionTokens = usage.value("completion_tokens", 0);
                    totalTokens = usage.value("total_tokens", 0);

                    chat->totalPromptTokens += promptTokens;
                    chat->totalCompletionTokens += completionTokens;
                    chat->totalTokens += totalTokens;
                }

                // If we were streaming, the message might already be there.
                // We should update it with final content and tokens.
                if (!chat->messages.empty() && chat->messages.back().role == "assistant") {
                    chat->messages.back().content = content;
                    chat->messages.back().promptTokens = promptTokens;
                    chat->messages.back().completionTokens = completionTokens;
                    chat->messages.back().totalTokens = totalTokens;
                } else if (!content.empty()) {
                    chat->messages.push_back({
                        "assistant", content, getTimeStr(), promptTokens, completionTokens, totalTokens
                    });
                }
            }

            size_t firstBrace = content.find('{');
            size_t lastBrace = content.rfind('}');

            if (firstBrace != std::string::npos && lastBrace != std::string::npos && lastBrace > firstBrace) {
                std::string jsonStr = content.substr(firstBrace, lastBrace - firstBrace + 1);
                try {
                    auto toolJson = nlohmann::json::parse(jsonStr);
                    if (toolJson.contains("tool") && (
                            toolJson.contains("arguments") || toolJson.contains("parameters"))) {
                        std::string toolKey = toolJson["tool"];
                        nlohmann::json args = toolJson.contains("arguments")
                                                  ? toolJson["arguments"]
                                                  : toolJson["parameters"];

                        LOG_INFO("AI requested tool call: " + toolKey);
                        std::string toolResult = executeTool(toolKey, args);

                        {
                            std::lock_guard<std::mutex> lock(repositoryMutex);
                            chat->messages.push_back({
                                "system", "Tool Result (" + toolKey + "): " + toolResult, getTimeStr()
                            });
                        }

                        sendFollowupRequest(chatId, model);
                        return;
                    }
                } catch (...) {
                    // Not a valid tool call JSON, just treat as regular message
                }
            }

            LOG_INFO("AI Response processed for chat " + chatId);
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to parse AI response: " + std::string(e.what()));
        }
    }

    void AIAssistantService::sendFollowupRequest(const std::string &chatId, AIModel model) {
        std::shared_ptr<Chat> currentChat = aiAssistantRepository->findChatById(chatId);
        if (!currentChat) return;

        std::string apiKey = getApiKey(model);
        if (apiKey.empty()) return;

        AIModelInfo modelInfo = getAIModelInfo(model);

        LOG_INFO("Sending Async Followup MCP request to: " + modelInfo.url + " using model " + modelInfo.modelId);

        nlohmann::json body;
        body["model"] = modelInfo.modelId;
        body["messages"] = buildMessages(currentChat);
        body["stream"] = true;

        std::string targetChatId = currentChat->id;
        std::string jsonBody = body.dump();
        std::string url = modelInfo.url + "/chat/completions";

        std::thread([this, url, jsonBody, apiKey, targetChatId, model]() {
            std::string accumulatedData;
            std::string currentContent;

            httpService->postStream(url, jsonBody, apiKey,
                                    [this, targetChatId, &accumulatedData, &currentContent](
                                const char *data, size_t len) {
                                        processStreamData(targetChatId, std::string(data, len), accumulatedData,
                                                          currentContent);
                                        return true;
                                    },
                                    [this, targetChatId, model](const std::string &fullResponse, bool success) {
                                        if (success) {
                                            LOG_INFO("Async Followup Request finished for " + targetChatId);
                                            processAIResponse(targetChatId, model, fullResponse);
                                        } else {
                                            LOG_ERROR("Async Followup Request failed: " + fullResponse);
                                        }
                                    });
        }).detach();
    }

    void AIAssistantService::processStreamData(const std::string &chatId, const std::string &chunk,
                                               std::string &accumulatedData, std::string &currentContent) {
        accumulatedData += chunk;

        size_t pos;
        while ((pos = accumulatedData.find('\n')) != std::string::npos) {
            std::string line = accumulatedData.substr(0, pos);
            accumulatedData.erase(0, pos + 1);

            if (line.empty()) continue;
            if (line.find("data: ") != 0) continue;

            std::string data = line.substr(6);
            if (data == "[DONE]") continue;

            try {
                auto j = nlohmann::json::parse(data);
                if (j.contains("choices") && !j["choices"].empty()) {
                    auto &choice = j["choices"][0];
                    if (choice.contains("delta") && choice["delta"].contains("content")) {
                        std::string delta = choice["delta"]["content"];
                        currentContent += delta;

                        // Update chat repository dynamically
                        std::lock_guard lock(repositoryMutex);
                        auto chat = aiAssistantRepository->findChatById(chatId);
                        if (chat) {
                            if (chat->messages.empty() || chat->messages.back().role != "assistant") {
                                chat->messages.push_back({"assistant", currentContent, getTimeStr()});
                            } else {
                                chat->messages.back().content = currentContent;
                            }
                        }
                    }
                }
            } catch (...) {
                // Ignore incomplete JSON chunks
            }
        }
    }

    std::string AIAssistantService::executeTool(const std::string &toolKey, const nlohmann::json &arguments) {
        for (auto *provider: toolProviders) {
            for (const auto &tool: provider->getTools()) {
                if (tool.key == toolKey) {
                    try {
                        return tool.callback(arguments);
                    } catch (const std::exception &e) {
                        return "Error executing tool: " + std::string(e.what());
                    }
                }
            }
        }
        return "Tool not found: " + toolKey;
    }

    std::string AIAssistantService::getApiKey(AIModel model) {
        if (!editorRepository) return "";

        AIModelInfo modelInfo = getAIModelInfo(model);
        bool isGpt = modelInfo.name.find("GPT") != std::string::npos;
        bool isGemini = modelInfo.name.find("Gemini") != std::string::npos;

        if (isGpt) {
            if (editorRepository->gptMcpKey.empty()) {
                if (notificationService) {
                    notificationService->pushMessage("No GPT key configured in Editor settings",
                                                     NotificationSeverities::ERROR);
                }
                return "";
            }
            return editorRepository->gptMcpKey;
        }

        if (isGemini) {
            if (editorRepository->geminiMcpKey.empty()) {
                if (notificationService) {
                    notificationService->pushMessage("No Gemini key configured in Editor settings",
                                                     NotificationSeverities::ERROR);
                }
                return "";
            }
            return editorRepository->geminiMcpKey;
        }

        return "";
    }

    std::string AIAssistantService::getTimeStr() {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%H:%M:%S");
        return oss.str();
    }

    std::string AIAssistantService::deleteCurrentChat(const std::string &chatId) {
        if (!aiAssistantRepository) return "";
        aiAssistantRepository->deleteChat(chatId);
        return aiAssistantRepository->createNewChat()->id;
    }

    void AIAssistantService::deleteMessagesFrom(const std::string &chatId, int messageIndex) {
        std::lock_guard<std::mutex> lock(repositoryMutex);
        auto chat = aiAssistantRepository->findChatById(chatId);
        if (chat && messageIndex >= 0 && (size_t)messageIndex < chat->messages.size()) {
            chat->messages.erase(chat->messages.begin() + messageIndex, chat->messages.end());
        }
    }

    void AIAssistantService::regenerateMessage(const std::string &chatId, int messageIndex, AIModel model) {
        {
            std::lock_guard<std::mutex> lock(repositoryMutex);
            auto chat = aiAssistantRepository->findChatById(chatId);
            if (!chat || messageIndex < 0 || (size_t)messageIndex >= chat->messages.size()) return;

            // Remove the assistant message (it should be an assistant message, but we remove it and everything after)
            chat->messages.erase(chat->messages.begin() + messageIndex, chat->messages.end());
        }
        sendFollowupRequest(chatId, model);
    }

    void AIAssistantService::editMessage(const std::string &chatId, int messageIndex, const std::string &newContent,
                                         AIModel model) {
        {
            std::lock_guard<std::mutex> lock(repositoryMutex);
            auto chat = aiAssistantRepository->findChatById(chatId);
            if (!chat || messageIndex < 0 || (size_t)messageIndex >= chat->messages.size()) return;

            // Update the message and remove everything after
            chat->messages[messageIndex].content = newContent;
            chat->messages[messageIndex].timestamp = getTimeStr();
            if ((size_t)messageIndex + 1 < chat->messages.size()) {
                chat->messages.erase(chat->messages.begin() + messageIndex + 1, chat->messages.end());
            }
        }
        sendFollowupRequest(chatId, model);
    }

    nlohmann::json AIAssistantService::buildMessages(const std::shared_ptr<Chat> &chat) {
        nlohmann::json messages = nlohmann::json::array();

        std::string fullSystemPrompt = buildFullSystemPrompt();
        if (!fullSystemPrompt.empty()) {
            messages.push_back({{"role", "system"}, {"content", fullSystemPrompt}});
        }

        for (const auto &msg: chat->messages) {
            messages.push_back({{"role", msg.role}, {"content", msg.content}});
        }

        return messages;
    }

    std::string AIAssistantService::buildFullSystemPrompt() {
        std::stringstream ss;
        if (!systemPrompt.empty()) {
            ss << systemPrompt << "\n\n";
        }

        if (!toolProviders.empty()) {
            ss << "AVAILABLE TOOLS:\n";
            ss <<
                    "You have access to the following tools. To use them, you must provide a JSON response in your output.\n\n";
            for (auto *provider: toolProviders) {
                for (const auto &tool: provider->getTools()) {
                    ss << tool.getSystemPrompt() << "\n";
                }
            }
        }

        return ss.str();
    }
}
