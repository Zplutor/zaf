#include "conversation_item_source.h"
#include "conversation_generating.h"
#include "conversation_item.h"

ConversationItemSource::ConversationItemSource() {

    conversations_ = GenerateConversations();
}


std::size_t ConversationItemSource::GetItemCount() {
    return conversations_.size();
}


float ConversationItemSource::GetItemHeight(std::size_t index) {
    return 64.f;
}


std::shared_ptr<zaf::ListControl::Item> ConversationItemSource::CreateItem(std::size_t index) {
    return zaf::Create<ConversationItem>();
}


void ConversationItemSource::LoadItem(std::size_t index, const std::shared_ptr<zaf::ListControl::Item>& item) {
   
    if (index >= conversations_.size()) {
        return;
    }

    auto conversation_item = std::dynamic_pointer_cast<ConversationItem>(item);
    if (conversation_item == nullptr) {
        return;
    }

    conversation_item->LoadConversation(conversations_[index]);
}