#include "AIAssistantRepository.h"
#include "common/Icons.h"
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

    void AIAssistantRepository::registerFields() {
        auto chatsToJson = [this] {
            nlohmann::json j = nlohmann::json::array();
            for (const auto &c: chats) {
                if (c) j.push_back(c->toJson());
            }
            return j;
        };

        auto chatsFromJson = [this](const nlohmann::json &j) {
            chats.clear();
            for (const auto &item: j) {
                auto c = std::make_shared<Chat>();
                c->fromJson(item);
                chats.push_back(c);
            }
        };

        registerGenericField(chatsToJson, chatsFromJson).setName("chats");
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

    void AIAssistantRepository::clear() {
        chats.clear();
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
