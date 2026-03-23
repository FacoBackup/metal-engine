#include "AIAssistantService.h"
#include "HttpService.h"
#include "NotificationService.h"
#include "../repository/EditorRepository.h"
#include "../repository/AIAssistantRepository.h"
#include "common/LoggerUtil.h"
#include "common/FilesUtil.h"
#include "ApplicationContext.h"
#include "../dto/IToolProvider.h"
#include <nlohmann/json.hpp>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "core/DirectoryService.h"

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
        if (apiKey.empty()) {
            return;
        }

        AIModelInfo modelInfo = getAIModelInfo(model);

        currentChat->messages.push_back({"user", message, getTimeStr(), false});

        currentChat->isProcessing = true;
        if (currentChat->messages.size() == 2) {
            std::string initialName = message.substr(0, 30);
            if (message.length() > 30) initialName += "...";
            currentChat->name = initialName;
        }

        LOG_INFO("Sending MCP request to: " + modelInfo.url + " using model " + modelInfo.modelId);

        nlohmann::json body;
        body["model"] = modelInfo.modelId;
        body["messages"] = buildMessages(currentChat);

        nlohmann::json tools = buildTools();
        if (!tools.empty()) {
            body["tools"] = tools;
        }

        std::string targetChatId = currentChat->id;
        std::string jsonBody = body.dump();
        currentChat->messages.emplace_back("assistant", "", getTimeStr(), true);

        directoryService->save(true);

        LOG_INFO("AIAssistantService: Request body: " + jsonBody);
        httpService->post(modelInfo.url + "/chat/completions", jsonBody, apiKey,
                          [this, targetChatId,currentChat, model](const std::string &response, bool success) {
                              int assistantMessageIndex = static_cast<int>(currentChat->messages.size() - 1);

                              if (success) {
                                  processAIResponse(targetChatId, assistantMessageIndex, model, response);
                                  currentChat->messages[assistantMessageIndex].isProcessing = false;
                                  directoryService->save(true);
                              } else {
                                  LOG_ERROR("MCP Request failed: " + response);
                                  if (notificationService) {
                                      notificationService->pushMessage("AI Request failed: " + response,
                                                                       NotificationSeverities::ERROR);
                                  }
                              }
                          });
    }

    void AIAssistantService::sendFollowupRequest(const std::string &chatId, AIModel model, int messageIndex) {
        std::shared_ptr<Chat> currentChat = aiAssistantRepository->findChatById(chatId);
        if (!currentChat) return;

        currentChat->isProcessing = true;

        std::string apiKey = getApiKey(model);
        if (apiKey.empty()) return;

        AIModelInfo modelInfo = getAIModelInfo(model);

        nlohmann::json body;
        body["model"] = modelInfo.modelId;
        body["messages"] = buildMessages(currentChat);

        nlohmann::json tools = buildTools();
        if (!tools.empty()) {
            body["tools"] = tools;
        }

        std::string jsonBody = body.dump();
        httpService->post(modelInfo.url + "/chat/completions", jsonBody, apiKey,
                          [this, chatId, messageIndex, model](const std::string &response, bool success) {
                              auto chat = aiAssistantRepository->findChatById(chatId);
                              if (chat) chat->isProcessing = false;

                              if (success) {
                                  processAIResponse(chatId, messageIndex, model, response);
                              } else {
                                  LOG_ERROR("Followup Request failed: " + response);
                              }
                          });
    }

    void AIAssistantService::regenerateMessage(const std::string &chatId, int messageIndex, AIModel model) {
        auto chat = aiAssistantRepository->findChatById(chatId);
        if (!chat || messageIndex < 0 || (size_t) messageIndex >= chat->messages.size()) return;

        chat->messages.erase(chat->messages.begin() + messageIndex - 1, chat->messages.end());
        sendRequest(chatId, chat->messages.back().content, model);
    }

    void AIAssistantService::editMessage(const std::string &chatId, int messageIndex, const std::string &newContent,
                                         const AIModel model) {
        auto chat = aiAssistantRepository->findChatById(chatId);
        if (!chat || messageIndex < 0 || static_cast<size_t>(messageIndex) >= chat->messages.size()) return;

        chat->messages.erase(chat->messages.begin() + messageIndex, chat->messages.end());
        sendRequest(chatId, newContent, model);
    }

    void AIAssistantService::deleteMessagesFrom(const std::string &chatId, int messageIndex) const {
        auto chat = aiAssistantRepository->findChatById(chatId);
        if (chat && messageIndex >= 0 && static_cast<size_t>(messageIndex) < chat->messages.size()) {
            chat->messages.erase(chat->messages.begin() + messageIndex, chat->messages.end());
        }
    }

    std::string AIAssistantService::deleteCurrentChat(const std::string &chatId) {
        if (!aiAssistantRepository) return "";
        aiAssistantRepository->deleteChat(chatId);
        if (!aiAssistantRepository->chats.empty()) {
            return aiAssistantRepository->chats.back()->id;
        }
        return aiAssistantRepository->createNewChat()->id;
    }

    nlohmann::json AIAssistantService::buildMessages(const std::shared_ptr<Chat> &chat) {
        nlohmann::json messages = nlohmann::json::array();

        std::string fullSystemPrompt = buildFullSystemPrompt();
        if (!fullSystemPrompt.empty()) {
            messages.push_back({{"role", "system"}, {"content", fullSystemPrompt}});
        }

        for (const auto &msg: chat->messages) {
            nlohmann::json m = {{"role", msg.role}, {"content", msg.content}};

            // Add tool calls if present (OpenAI/Gemini native format)
            if (!msg.toolCalls.empty()) {
                nlohmann::json toolCallsJson = nlohmann::json::array();
                for (const auto &tool: msg.toolCalls) {
                    if (!tool.rawJson.empty()) {
                        toolCallsJson.push_back(nlohmann::json::parse(tool.rawJson));
                    } else {
                        nlohmann::json tc;
                        tc["id"] = tool.id;
                        tc["type"] = "function";
                        tc["function"] = {
                            {"name", tool.name},
                            {"arguments", tool.description}
                        };
                        toolCallsJson.push_back(tc);
                    }
                }
                m["tool_calls"] = toolCallsJson;
            }

            messages.push_back(m);

            // Add tool results as separate messages with role "tool"
            if (!msg.toolCalls.empty()) {
                for (const auto &tool: msg.toolCalls) {
                    if (!tool.result.empty()) {
                        nlohmann::json toolResponse;
                        toolResponse["role"] = "tool";
                        toolResponse["tool_call_id"] = tool.id;
                        toolResponse["name"] = tool.name;
                        toolResponse["content"] = tool.result;
                        messages.push_back(toolResponse);
                    }
                }
            }
        }

        return messages;
    }

    nlohmann::json AIAssistantService::buildTools() {
        nlohmann::json toolsArray = nlohmann::json::array();
        for (auto *provider: toolProviders) {
            for (const auto &tool: provider->getTools()) {
                nlohmann::json toolJson = nlohmann::json::parse(tool.getSystemPrompt());
                nlohmann::json toolWrapper;
                toolWrapper["type"] = "function";
                
                // Convert MCP-style JSON to OpenAI-style tool call if needed.
                // MCP: { name, description, inputSchema }
                // OpenAI: { type: "function", function: { name, description, parameters } }
                nlohmann::json functionObj;
                functionObj["name"] = toolJson["name"];
                functionObj["description"] = toolJson["description"];
                functionObj["parameters"] = toolJson["inputSchema"];
                
                toolWrapper["function"] = functionObj;
                toolsArray.push_back(toolWrapper);
            }
        }
        return toolsArray;
    }

    std::string AIAssistantService::buildFullSystemPrompt() {
        return systemPrompt;
    }

    void AIAssistantService::processAIResponse(const std::string &chatId, int messageIndex, AIModel model,
                                               const std::string &response) {
        try {
            auto responseJson = nlohmann::json::parse(response);

            auto chat = aiAssistantRepository->findChatById(chatId);
            if (!chat || messageIndex < 0 || static_cast<size_t>(messageIndex) >= chat->messages.size()) return;

            auto &assistantMsg = chat->messages[messageIndex];

            if (!responseJson.contains("choices") || responseJson["choices"].empty()) {
                chat->isProcessing = false;
                return;
            }

            auto firstChoice = responseJson["choices"][0];
            if (!firstChoice.contains("message")) {
                chat->isProcessing = false;
                return;
            }

            LOG_INFO("AIAssistantService: Processing response... " + responseJson.dump(4));
            auto message = firstChoice["message"];
            std::string content = message.value("content", "");
            LOG_INFO("AIAssistantService: Content: " + content);

            int promptTokens = 0;
            int completionTokens = 0;
            int totalTokens = 0;

            if (responseJson.contains("usage")) {
                auto usage = responseJson["usage"];
                promptTokens = usage.value("prompt_tokens", 0);
                completionTokens = usage.value("completion_tokens", 0);
                totalTokens = usage.value("total_tokens", 0);
            }

            chat->totalPromptTokens += promptTokens;
            chat->totalCompletionTokens += completionTokens;
            chat->totalTokens += totalTokens;

            assistantMsg.promptTokens += promptTokens;
            assistantMsg.completionTokens += completionTokens;
            assistantMsg.totalTokens += totalTokens;
            assistantMsg.timestamp = getTimeStr();
            assistantMsg.content += content;

            // Check for native tool_calls (as per OpenAI and Gemini MCP formats)
            if (message.contains("tool_calls") && message["tool_calls"].is_array() && !message["tool_calls"].empty()) {
                LOG_INFO("AIAssistantService: Found native tool_calls array");
                for (const auto& toolCall : message["tool_calls"]) {
                    if (toolCall.contains("function")) {
                        auto function = toolCall["function"];
                        std::string action = function.value("name", "");
                        std::string argumentsStr = function.value("arguments", "{}");
                        std::string toolCallId = toolCall.value("id", "");
                        
                        try {
                            nlohmann::json arguments;
                            if (argumentsStr.empty() || argumentsStr == "null") {
                                arguments = nlohmann::json::object();
                            } else {
                                arguments = nlohmann::json::parse(argumentsStr);
                            }
                            
                            LOG_INFO("AIAssistantService: Executing native tool: " + action + " (ID: " + toolCallId + ")");
                            std::string result = executeTool(action, arguments);
                            assistantMsg.toolCalls.emplace_back(toolCallId, action, argumentsStr, result, toolCall.dump());
                        } catch (const std::exception& e) {
                            LOG_ERROR("AIAssistantService: Error parsing tool arguments: " + std::string(e.what()));
                        }
                    }
                }
                
                if (!assistantMsg.toolCalls.empty()) {
                    sendFollowupRequest(chatId, model, messageIndex);
                    return;
                }
            }

            assistantMsg.content = content;
            chat->isProcessing = false;
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to parse AI response: " + std::string(e.what()));
        }
    }

    std::string AIAssistantService::executeTool(const std::string &toolKey, const nlohmann::json &arguments) const {
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

    std::string AIAssistantService::getApiKey(const AIModel model) {
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
}
