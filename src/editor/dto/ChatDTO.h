#ifndef CHAT_DTO_H
#define CHAT_DTO_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#include "../../common/ISerialize.h"

namespace Metal {
    struct ChatMessage final : ISerialize {
        std::string role;
        std::string content;
        std::string timestamp;
        int promptTokens = 0;
        int completionTokens = 0;
        int totalTokens = 0;

        ChatMessage() = default;
        ChatMessage(std::string r, std::string c, std::string t, int p = 0, int comp = 0, int tot = 0)
            : role(std::move(r)), content(std::move(c)), timestamp(std::move(t)), 
              promptTokens(p), completionTokens(comp), totalTokens(tot) {}

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["role"] = role;
            j["content"] = content;
            j["timestamp"] = timestamp;
            j["promptTokens"] = promptTokens;
            j["completionTokens"] = completionTokens;
            j["totalTokens"] = totalTokens;
            return j;
        }

        void fromJson(const nlohmann::json &j) override {
            if (j.contains("role")) role = j.at("role").get<std::string>();
            if (j.contains("content")) content = j.at("content").get<std::string>();
            if (j.contains("timestamp")) timestamp = j.at("timestamp").get<std::string>();
            promptTokens = j.value("promptTokens", 0);
            completionTokens = j.value("completionTokens", 0);
            totalTokens = j.value("totalTokens", 0);
        }
    };

    struct Chat final : ISerialize {
        std::string id;
        std::string name;
        std::string date;
        std::vector<ChatMessage> messages;
        int totalPromptTokens = 0;
        int totalCompletionTokens = 0;
        int totalTokens = 0;

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["id"] = id;
            j["name"] = name;
            j["date"] = date;
            j["totalPromptTokens"] = totalPromptTokens;
            j["totalCompletionTokens"] = totalCompletionTokens;
            j["totalTokens"] = totalTokens;

            nlohmann::json messagesJson = nlohmann::json::array();
            for (const auto& msg : messages) {
                messagesJson.push_back(msg.toJson());
            }
            j["messages"] = messagesJson;
            return j;
        }

        void fromJson(const nlohmann::json &j) override {
            if (j.contains("id")) id = j.at("id").get<std::string>();
            if (j.contains("name")) name = j.at("name").get<std::string>();
            if (j.contains("date")) date = j.at("date").get<std::string>();
            totalPromptTokens = j.value("totalPromptTokens", 0);
            totalCompletionTokens = j.value("totalCompletionTokens", 0);
            totalTokens = j.value("totalTokens", 0);

            if (j.contains("messages") && j.at("messages").is_array()) {
                messages.clear();
                for (const auto& msgJson : j.at("messages")) {
                    ChatMessage msg;
                    msg.fromJson(msgJson);
                    messages.push_back(msg);
                }
            }
        }
    };
}

#endif // CHAT_DTO_H
