#include "ui/main/conversation/list/conversation_item_source.h"
#include <algorithm>
#include "logic/service.h"
#include "ui/main/conversation/list/conversation_item.h"

static bool ConversationComparer(
    const std::shared_ptr<Conversation>& conversation1,
    const std::shared_ptr<Conversation>& conversation2) {
    return conversation1->last_updated_time > conversation2->last_updated_time;
}


ConversationItemSource::ConversationItemSource() {

    Service::GetInstance().GetConversationUpdateEvent().AddListenerWithTag(
        reinterpret_cast<std::uintptr_t>(this),
        std::bind(&ConversationItemSource::ConversationUpdate, this, std::placeholders::_1));
}


ConversationItemSource::~ConversationItemSource() {

    Service::GetInstance().GetConversationUpdateEvent().RemoveListenersWithTag(reinterpret_cast<std::uintptr_t>(this));
}


void ConversationItemSource::LoadConversations(const std::shared_ptr<ConversationAvatarManager>& avatar_manager) {

    conversations_ = Service::GetInstance().GetAllConversations();
    std::sort(conversations_.begin(), conversations_.end(), ConversationComparer);

    conversation_avatar_manager_ = avatar_manager;
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

    auto conversation = conversations_[index];
    auto avatar_image = conversation_avatar_manager_->GetConversationAvatarImage(conversation);
    conversation_item->LoadConversation(conversation, avatar_image);
}


void ConversationItemSource::ConversationUpdate(const std::shared_ptr<Conversation>& updated_conversation) {

    auto iterator = std::find_if(
        conversations_.begin(),
        conversations_.end(),
        [&updated_conversation](const std::shared_ptr<Conversation>& conversation) {
            return conversation->id == updated_conversation->id;
        }
    );

    if (iterator == conversations_.end()) {
        return;
    }

    std::size_t old_index = std::distance(conversations_.begin(), iterator);
    conversations_.erase(iterator);
    NotifyItemRemove(old_index, 1);

    auto insert_iterator = std::lower_bound(
        conversations_.begin(),
        conversations_.end(),
        updated_conversation, 
        ConversationComparer);

    insert_iterator = conversations_.insert(insert_iterator, updated_conversation);

    std::size_t new_index = std::distance(conversations_.begin(), insert_iterator);
    NotifyItemAdd(new_index, 1);
}