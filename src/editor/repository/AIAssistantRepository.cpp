#include "AIAssistantRepository.h"
#include "../../common/Icons.h"
#include <ctime>
#include <iomanip>
#include <sstream>

namespace Metal {
    const char *AIAssistantRepository::getTitle() const {
        return "Chats";
    }

    const char *AIAssistantRepository::getIcon() const {
        return Icons::comment.c_str();
    }

    nlohmann::json AIAssistantRepository::toJson() const {
        nlohmann::json j;
        nlohmann::json chatsJson = nlohmann::json::array();
        for (const auto& chat : chats) {
            if (chat) {
                chatsJson.push_back(chat->toJson());
            }
        }
        j["chats"] = chatsJson;
        return j;
    }

    void AIAssistantRepository::fromJson(const nlohmann::json &j) {
        if (j.contains("chats") && j.at("chats").is_array()) {
            chats.clear();
            for (const auto& chatJson : j.at("chats")) {
                auto chat = std::make_shared<Chat>();
                chat->fromJson(chatJson);
                chats.push_back(chat);
            }
        }
    }

    std::shared_ptr<Chat> AIAssistantRepository::findChatById(const std::string &id) {
        for (auto &chat: chats) {
            if (chat && chat->id == id) {
                return chat;
            }
        }
        return nullptr;
    }

    void AIAssistantRepository::addChat(const std::shared_ptr<Chat> &chat) {
        chats.push_back(chat);
    }

    void AIAssistantRepository::deleteChat(const std::string &id) {
        chats.erase(std::remove_if(chats.begin(), chats.end(),
                                   [&id](const std::shared_ptr<Chat> &c) { return c && c->id == id; }), chats.end());
    }

    std::shared_ptr<Chat> AIAssistantRepository::createNewChat() {
        auto chat = std::make_shared<Chat>();
        chat->id = Util::uuidV4();
        chat->name = "New Chat";

        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%H:%M:%S");
        chat->date = oss.str();

        addChat(chat);
        return chat;
    }
}
