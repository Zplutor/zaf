#include "storage/message_storage.h"
#include <algorithm>
#include "storage/id_generating.h"

std::vector<std::shared_ptr<Message>> MessageStorage::GetMessages(const std::vector<Id>& message_ids) {

    std::vector<std::shared_ptr<Message>> messages;

    for (auto each_id : message_ids) {

        auto iterator = messages_.find(each_id);
        if (iterator != messages_.end()) {
            auto message = std::make_shared<Message>(*(iterator->second));
            messages.push_back(message);
        }
    }

    return messages;
}


std::vector<std::shared_ptr<Message>> MessageStorage::GetConversationMessages(Id conversation_id) {

    std::vector<std::shared_ptr<Message>> messages;

    auto iterator = conversation_messages_.find(conversation_id);
    if (iterator != conversation_messages_.end()) {
        messages = GetMessages(iterator->second);
    }

    return messages;
}


std::shared_ptr<Message> MessageStorage::GetConversationLastMessage(Id conversation_id) {

    auto iterator = conversation_last_message_.find(conversation_id);
    if (iterator != conversation_last_message_.end()) {

        auto messages = GetMessages({ iterator->second });
        if (! messages.empty()) {
            return messages.front();
        }
        return nullptr;
    }

    auto conversation_messages = GetConversationMessages(conversation_id);
    if (conversation_messages.empty()) {
        return nullptr;
    }

    std::sort(
        conversation_messages.begin(),
        conversation_messages.end(),
        [](const std::shared_ptr<Message>& message1, const std::shared_ptr<Message>& message2) {
            return message1->sent_time > message2->sent_time;
        }
    );

    auto last_message = conversation_messages.front();
    conversation_last_message_.insert(std::make_pair(conversation_id, last_message->id));
    return last_message;
}


std::vector<Id> MessageStorage::AddMessages(const std::vector<std::shared_ptr<Message>>& messages) {

    std::vector<Id> message_ids;

    //Add to all messages, and group by conversation.
    std::map<Id, std::vector<std::shared_ptr<Message>>> messages_group_by_conversation;

    for (const auto& each_message : messages) {

        auto new_message = std::make_shared<Message>(*each_message);
        new_message->id = GenerateId();
        messages_.insert(std::make_pair(new_message->id, new_message));
        message_ids.push_back(new_message->id);

        messages_group_by_conversation[new_message->conversation_id].push_back(new_message);
    }

    for (auto& each_pair : messages_group_by_conversation) {

        auto& messages_to_add = each_pair.second;

        //Add to conversation messages.
        auto& messages_in_conversation = conversation_messages_[each_pair.first];
        messages_in_conversation.insert(
            messages_in_conversation.end(),
            messages_to_add.begin(),
            messages_to_add.end());

        //Sort messages by sent time, get last message.
        std::sort(
            messages_to_add.begin(),
            messages_to_add.end(),
            [](const std::shared_ptr<Message>& message1, const std::shared_ptr<Message>& message2) {
                return message1->sent_time > message2->sent_time;
            }
        );
        auto last_message = messages_to_add.front();

        //Update conversation last message
        auto iterator = conversation_last_message_.find(each_pair.first);
        if (iterator != conversation_last_message_.end()) {

            auto current_last_message = GetMessages({ iterator->second });
            if (! current_last_message.empty()) {
                if (current_last_message.front()->sent_time < last_message->sent_time) {
                    iterator->second = last_message->id;
                }
            }
        }
        else {
            conversation_last_message_.insert(std::make_pair(each_pair.first, last_message->id));
        }
    }

    return message_ids;
}


bool MessageStorage::AddUnreadMessage(Id message_id) {

    auto messages = GetMessages({ message_id });
    if (messages.empty()) {
        return false;
    }

    auto message = messages.front();
    conversation_unread_messages_[message->conversation_id].push_back(message->id);
    return true;
}


std::size_t MessageStorage::GetConversationUnreadMessageCount(Id conversation_id) {

    auto iterator = conversation_unread_messages_.find(conversation_id);
    if (iterator != conversation_unread_messages_.end()) {
        return iterator->second.size();
    }
    return 0;
}