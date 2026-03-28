#ifndef CHAT_DTO_H
#define CHAT_DTO_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#include "common/Reflection.h"

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
            registerSerializableOnlyField(&id, STRING, "id");
            registerSerializableOnlyField(&name, STRING, "name");
            registerSerializableOnlyField(&description, STRING, "description");
            registerSerializableOnlyField(&result, STRING, "result");
            registerSerializableOnlyField(&rawJson, STRING, "rawJson");
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
            registerSerializableOnlyField(&role, STRING, "role");
            registerSerializableOnlyField(&content, STRING, "content");
            registerSerializableOnlyField(&timestamp, STRING, "timestamp");
            registerSerializableOnlyField(&promptTokens, INT, "promptTokens");
            registerSerializableOnlyField(&completionTokens, INT, "completionTokens");
            registerSerializableOnlyField(&totalTokens, INT, "totalTokens");
            registerSerializableOnlyField(&renderedHeight, FLOAT, "renderedHeight");
            registerSerializableOnlyField(&toolCalls, COMPOSITE, "toolCalls");
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

    protected:
        void registerFields() override {
            registerSerializableOnlyField(&id, STRING, "id");
            registerSerializableOnlyField(&name, STRING, "name");
            registerSerializableOnlyField(&date, STRING, "date");
            registerSerializableOnlyField(&totalPromptTokens, INT, "totalPromptTokens");
            registerSerializableOnlyField(&totalCompletionTokens, INT, "totalCompletionTokens");
            registerSerializableOnlyField(&totalTokens, INT, "totalTokens");
            registerSerializableOnlyField(&messages, COMPOSITE, "messages");
        }
    };
}

#endif // CHAT_DTO_H
