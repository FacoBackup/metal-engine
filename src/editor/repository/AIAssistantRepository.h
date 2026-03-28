#ifndef aiAssistantRepository_H
#define aiAssistantRepository_H

#include "common/IRepository.h"
#include "../dto/ChatDTO.h"
#include <vector>
#include <string>
#include <memory>

namespace Metal {
    struct AIAssistantRepository final : IRepository {
        std::vector<std::shared_ptr<Chat> > chats;

        AIAssistantRepository() = default;

        void registerFields() override;

        const char *getTitle() const override;

        const char *getIcon() const override;

        std::shared_ptr<Chat> findChatById(const std::string &id);

        void addChat(const std::shared_ptr<Chat> &chat);

        void deleteChat(const std::string &id);

        std::shared_ptr<Chat> createNewChat();

        void clear() override;
    };
}

#endif // aiAssistantRepository_H
