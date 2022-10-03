#pragma once

#include <map>
#include <memory>
#include <vector>
#include "entity/conversation.h"

class ConversationStorage {
public:
    ConversationStorage() { }
    ConversationStorage(const ConversationStorage&) = delete;
    ConversationStorage& operator=(const ConversationStorage&) = delete;

    std::vector<std::shared_ptr<Conversation>> GetAllConversations();
    std::shared_ptr<Conversation> GetConversaton(Id conversation_id);
    std::vector<Id> AddConversations(const std::vector<std::shared_ptr<Conversation>>& conversations);
    bool UpdateConversation(const std::shared_ptr<Conversation>& conversation);

private:
    std::map<Id, std::shared_ptr<Conversation>> conversations_;
};