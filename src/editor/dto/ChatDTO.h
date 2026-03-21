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

        ChatMessage() = default;
        ChatMessage(std::string r, std::string c, std::string t)
            : role(std::move(r)), content(std::move(c)), timestamp(std::move(t)) {}

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["role"] = role;
            j["content"] = content;
            j["timestamp"] = timestamp;
            return j;
        }

        void fromJson(const nlohmann::json &j) override {
            if (j.contains("role")) role = j.at("role").get<std::string>();
            if (j.contains("content")) content = j.at("content").get<std::string>();
            if (j.contains("timestamp")) timestamp = j.at("timestamp").get<std::string>();
        }
    };

    struct Chat final : ISerialize {
        std::string id;
        std::string name;
        std::string date;
        std::vector<ChatMessage> messages;

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["id"] = id;
            j["name"] = name;
            j["date"] = date;

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
