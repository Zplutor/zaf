#include "ui/main/conversation/detail/chat/message/conversation_message_list_view.h"
#include "logic/service.h"
#include "ui/main/conversation/detail/chat/message/message_item.h"

void ConversationMessageListView::Initialize() {

    __super::Initialize();

    SetSelectionMode(SelectionMode::None);
    SetAllowHorizontalScroll(false);
    SetBackgroundColor(zaf::Color::FromRGB(0xECF0F3));
    GetItemContainer()->SetPadding(zaf::Frame(15, 0, 15, 13));
    SetItemSource(std::dynamic_pointer_cast<zaf::ListControl::ItemSource>(shared_from_this()));
}


void ConversationMessageListView::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    NotifyItemUpdate(0, message_item_infos_.size());
}


void ConversationMessageListView::SetConversation(const std::shared_ptr<Conversation>& conversation) {

    conversation_id_ = conversation->id;
    auto messages = Service::GetInstance().GetConversationMessages(conversation_id_);

    message_item_infos_.clear();
    for (const auto& each_message : messages) {

        auto message_item = zaf::Create<MessageItem>();
        message_item->SetMessage(each_message);

        auto message_item_info = std::make_shared<MessageItemInfo>();
        message_item_info->message = each_message;
        message_item_info->message_item = message_item;
        message_item_infos_.push_back(message_item_info);
    }

    Reload();
    ScrollToItemAtIndex(message_item_infos_.size() - 1);
}


std::size_t ConversationMessageListView::GetItemCount() {
    return message_item_infos_.size();
}


bool ConversationMessageListView::HasVariableItemHeight() {
    return true;
}


float ConversationMessageListView::GetItemHeight(std::size_t index) {

    if (index >= message_item_infos_.size()) {
        return 0;
    }
    
    return message_item_infos_[index]->message_item->DeterminateHeight(GetItemContainer()->GetContentSize().width);
}


std::shared_ptr<zaf::ListControl::Item> ConversationMessageListView::CreateItem(std::size_t index) {

    if (index >= message_item_infos_.size()) {
        return nullptr;
    }

    return message_item_infos_[index]->message_item;
}
