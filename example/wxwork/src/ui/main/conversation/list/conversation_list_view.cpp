#include "conversation_list_view.h"
#include <algorithm>
#include <zaf/control/scroll_bar.h>
#include "logic/service.h"
#include "ui/main/conversation/list/conversation_item.h"

inline bool ConversationComparer(
    const std::shared_ptr<Conversation>& conversation1,
    const std::shared_ptr<Conversation>& conversation2) {
    return conversation1->last_updated_time > conversation2->last_updated_time;
}


ConversationListView::~ConversationListView() {

}


void ConversationListView::Initialize() {

    __super::Initialize();

    SetBorder(zaf::Frame(0));
    SetAllowHorizontalScroll(false);
    SetAutoAdjustScrollBarSmallChange(false);
    SetAutoAdjustScrollBarLargeChange(false);

    auto vertical_scroll_bar = GetVerticalScrollBar();
    vertical_scroll_bar->SetSmallChange(64);
    vertical_scroll_bar->SetLargeChange(64 * 10);

    SetDataSource(std::dynamic_pointer_cast<zaf::ListDataSource>(shared_from_this()));
    SetDelegate(std::dynamic_pointer_cast<zaf::ListControlDelegate>(shared_from_this()));
}


std::size_t ConversationListView::GetDataCount() {
    return conversations_.size();
}


float ConversationListView::EstimateItemHeight(
    std::size_t index, 
    const std::shared_ptr<zaf::Object>& data) {

    return 64;
}


std::shared_ptr<zaf::ListItem> ConversationListView::CreateItem(
    std::size_t index,
    const std::shared_ptr<zaf::Object>& data) {

    return zaf::Create<ConversationItem>();
}


void ConversationListView::LoadItem(
    const std::shared_ptr<zaf::ListItem>& item,
    std::size_t index, 
    const std::shared_ptr<zaf::Object>& data) {

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


void ConversationListView::LoadConversations() {

    conversations_ = Service::GetInstance().GetAllConversations();
    std::sort(conversations_.begin(), conversations_.end(), ConversationComparer);

    Reload();

    Subscriptions() += Service::GetInstance().GetConversationUpdateEvent().Subscribe(
        std::bind(&ConversationListView::ConversationUpdate, this, std::placeholders::_1));
}


void ConversationListView::ConversationUpdate(const std::shared_ptr<Conversation>& updated_conversation) {

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
    bool is_selected = IsItemSelectedAtIndex(old_index);

    conversations_.erase(iterator);
    NotifyDataRemove(old_index, 1);

    auto insert_iterator = std::lower_bound(
        conversations_.begin(),
        conversations_.end(),
        updated_conversation,
        ConversationComparer);

    insert_iterator = conversations_.insert(insert_iterator, updated_conversation);

    std::size_t new_index = std::distance(conversations_.begin(), insert_iterator);
    NotifyDataAdd(new_index, 1);

    if (is_selected) {
        SelectItemAtIndex(new_index);
    }
}


std::shared_ptr<Conversation> ConversationListView::GetSelectedConversation() {

    auto selected_index = GetFirstSelectedItemIndex();
    if (!selected_index) {
        return nullptr;
    }

    if (*selected_index >= conversations_.size()) {
        return nullptr;
    }

    return conversations_[*selected_index];
}