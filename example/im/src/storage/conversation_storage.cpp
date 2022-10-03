#include "storage/conversation_storage.h"
#include "storage/id_generating.h"

std::vector<std::shared_ptr<Conversation>> ConversationStorage::GetAllConversations() {

    std::vector<std::shared_ptr<Conversation>> conversations;
    for (const auto& each_conversation : conversations_) {

        auto new_conversation = std::make_shared<Conversation>(*each_conversation.second);
        conversations.push_back(new_conversation);
    }
    return conversations;
}


std::shared_ptr<Conversation> ConversationStorage::GetConversaton(Id conversation_id) {

    auto iterator = conversations_.find(conversation_id);
    if (iterator == conversations_.end()) {
        return nullptr;
    }

    return std::make_shared<Conversation>(*iterator->second);
}


std::vector<Id> ConversationStorage::AddConversations(const std::vector<std::shared_ptr<Conversation>>& conversations) {

    std::vector<Id> added_ids;

    for (const auto& each_conversation : conversations) {

        auto new_conversation = std::make_shared<Conversation>(*each_conversation);
        new_conversation->id = GenerateId();

        conversations_.insert(std::make_pair(new_conversation->id, new_conversation));
        added_ids.push_back(new_conversation->id);
    }

    return added_ids;
}


bool ConversationStorage::UpdateConversation(const std::shared_ptr<Conversation>& conversation) {

    auto iterator = conversations_.find(conversation->id);
    if (iterator == conversations_.end()) {
        return false;
    }

    auto new_conversation = std::make_shared<Conversation>(*conversation);
    iterator->second = new_conversation;
    return true;
}