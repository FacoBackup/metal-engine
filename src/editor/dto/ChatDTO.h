#ifndef CHAT_DTO_H
#define CHAT_DTO_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#include "common/Reflection.h"
#include "Skill.h"

namespace Metal {
    struct ToolCall final : Reflection {
        std::string id;
        std::string name;
        std::string description;
        std::string result;
        std::string rawJson; // Stores the original tool call JSON to preserve metadata like thought_signature

        ToolCall() = default;

        ToolCall(std::string id, std::string n, std::string d, std::string r = "", std::string rj = "")
            : id(std::move(id)), name(std::move(n)), description(std::move(d)), result(std::move(r)),
              rawJson(std::move(rj)) {
        }

    protected:
        void registerFields() override {
            registerSerializableOnlyField<STRING>(&id).setName("id");
            registerSerializableOnlyField<STRING>(&name).setName("name");
            registerSerializableOnlyField<STRING>(&description).setName("description");
            registerSerializableOnlyField<STRING>(&result).setName("result");
            registerSerializableOnlyField<STRING>(&rawJson).setName("rawJson");
        }
    };

    struct ChatMessage final : Reflection {
        std::string role;
        std::string content;
        std::string timestamp;
        int promptTokens = 0;
        int completionTokens = 0;
        int totalTokens = 0;
        float renderedHeight = 0.0f;
        std::vector<ToolCall> toolCalls;
        bool isProcessing = false;

        ChatMessage() = default;

        ChatMessage(std::string r, std::string c, std::string t, bool isProcessing, int p = 0, int comp = 0,
                    int tot = 0)
            : role(std::move(r)), content(std::move(c)), timestamp(std::move(t)),
              promptTokens(p), completionTokens(comp), totalTokens(tot), isProcessing(isProcessing),
              renderedHeight(0.0f) {
        }

    protected:
        void registerFields() override {
            registerSerializableOnlyField<STRING>(&role).setName("role");
            registerSerializableOnlyField<STRING>(&content).setName("content");
            registerSerializableOnlyField<STRING>(&timestamp).setName("timestamp");
            registerSerializableOnlyField<INT>(&promptTokens).setName("promptTokens");
            registerSerializableOnlyField<INT>(&completionTokens).setName("completionTokens");
            registerSerializableOnlyField<INT>(&totalTokens).setName("totalTokens");
            registerSerializableOnlyField<FLOAT>(&renderedHeight).setName("renderedHeight");

            auto toolCallsToJson = [this] {
                nlohmann::json j = nlohmann::json::array();
                for (auto &tc: toolCalls) j.push_back(tc.toJson());
                return j;
            };

            auto toolCallsFromJson = [this](const nlohmann::json &j) {
                toolCalls.clear();
                for (const auto &item: j) {
                    ToolCall tc;
                    tc.fromJson(item);
                    toolCalls.push_back(tc);
                }
            };

            registerGenericField(toolCallsToJson, toolCallsFromJson).setName("toolCalls");
        }
    };

    struct Chat final : Reflection {
        bool isProcessing = false;
        std::string id;
        std::string name;
        std::string date;
        std::vector<ChatMessage> messages;
        int totalPromptTokens = 0;
        int totalCompletionTokens = 0;
        int totalTokens = 0;
        const Skill* selectedSkill = nullptr;

        void setSelectedSkill(const Skill* skill) {
            selectedSkill = skill;
        }

    protected:
        void registerFields() override {
            registerSerializableOnlyField<STRING>(&id).setName("id");
            registerSerializableOnlyField<STRING>(&name).setName("name");
            registerSerializableOnlyField<STRING>(&date).setName("date");
            registerSerializableOnlyField<INT>(&totalPromptTokens).setName("totalPromptTokens");
            registerSerializableOnlyField<INT>(&totalCompletionTokens).setName("totalCompletionTokens");
            registerSerializableOnlyField<INT>(&totalTokens).setName("totalTokens");

            auto messagesToJson = [this] {
                nlohmann::json j = nlohmann::json::array();
                for (auto &m: messages) j.push_back(m.toJson());
                return j;
            };

            auto messagesFromJson = [this](const nlohmann::json &j) {
                messages.clear();
                for (const auto &item: j) {
                    ChatMessage m;
                    m.fromJson(item);
                    messages.push_back(m);
                }
            };

            registerGenericField(messagesToJson, messagesFromJson).setName("messages");
        }
    };
}

#endif // CHAT_DTO_H
