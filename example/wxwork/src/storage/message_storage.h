#pragma once

#include <map>
#include <vector>
#include "entity/message.h"

class MessageStorage {
public:
    MessageStorage() { }
    MessageStorage(const MessageStorage&) { }
    MessageStorage& operator=(const MessageStorage&) { }

    std::vector<std::shared_ptr<Message>> GetMessages(const std::vector<Id>& message_ids);

    std::vector<std::shared_ptr<Message>> GetConversationMessages(Id conversation_id);
    std::shared_ptr<Message> GetConversationLastMessage(Id conversation_id);

    std::vector<Id> AddMessages(const std::vector<std::shared_ptr<Message>>& messages);

    Id AddMessage(const std::shared_ptr<Message>& message) {
        auto added_message_ids = AddMessages({ message });
        return added_message_ids.empty() ? InvalidId : added_message_ids.front();
    }

    bool AddUnreadMessage(Id message_id);
    std::size_t GetConversationUnreadMessageCount(Id conversation_id);

private:
    std::map<Id, std::shared_ptr<Message>> messages_;

    std::map<Id, std::vector<Id>> conversation_messages_;
    std::map<Id, Id> conversation_last_message_;

    std::map<Id, std::vector<Id>> conversation_unread_messages_;
};