#include "ui/main/conversation/detail/chat/message/conversation_message_list_view.h"
#include <algorithm>
#include <zaf/control/list_item_container.h>
#include <zaf/control/scroll_bar.h>
#include "logic/service.h"
#include "ui/main/conversation/detail/chat/message/message_item.h"

inline bool MessageComparer(
    const std::shared_ptr<Message>& message1, 
    const std::shared_ptr<Message>& message2) {

    if (message1->sent_time < message2->sent_time) {
        return true;
    }

    if (message1->sent_time > message2->sent_time) {
        return false;
    }

    return message1->id < message2->id;
}


ConversationMessageListView::~ConversationMessageListView() {

}


void ConversationMessageListView::Initialize() {

    __super::Initialize();

    SetBorder(zaf::Frame(0));
    SetSelectionMode(zaf::SelectionMode::None);
    SetAllowHorizontalScroll(false);
    SetAutoAdjustScrollBarSmallChange(false);
    SetAutoScrollBarLargeChange(false);
    SetBackgroundColor(zaf::Color::FromRGB(0xECF0F3));
    ItemContainer()->SetPadding(zaf::Frame(15, 0, 15, 13));
    SetDataSource(std::dynamic_pointer_cast<zaf::ListDataSource>(shared_from_this()));
    SetDelegate(std::dynamic_pointer_cast<zaf::ListControlDelegate>(shared_from_this()));

    auto scroll_bar = VerticalScrollBar();
    scroll_bar->SetSmallChange(14);

    Subscriptions() += Service::GetInstance().GetMessageAddEvent().Subscribe(
        std::bind(&ConversationMessageListView::OnMessageAdd, this, std::placeholders::_1));
}


void ConversationMessageListView::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    //Update all items' height if width is changed.
    if (previous_rect.size.width != Size().width) {
        NotifyDataUpdate(0, message_item_infos_.size());
    }
}


void ConversationMessageListView::SetConversation(const std::shared_ptr<Conversation>& conversation) {

    conversation_id_ = conversation->id;

    auto messages = Service::GetInstance().GetConversationMessages(conversation_id_);
    std::sort(messages.begin(), messages.end(), MessageComparer);

    message_item_infos_.clear();
    for (const auto& each_message : messages) {

        auto message_item_info = CreateMessageItemInfo(each_message);
        message_item_infos_.push_back(message_item_info);
    }

    Reload();
    ScrollToBottom();

    Service::GetInstance().RemoveConversationAllUnreadMessages(conversation_id_);
}


std::shared_ptr<ConversationMessageListView::MessageItemInfo> 
    ConversationMessageListView::CreateMessageItemInfo(const std::shared_ptr<Message>& message) {

    auto message_item = zaf::Create<MessageItem>();
    message_item->SetMessage(message);

    auto message_item_info = std::make_shared<MessageItemInfo>();
    message_item_info->message = message;
    message_item_info->message_item = message_item;

    return message_item_info;
}


void ConversationMessageListView::OnMessageAdd(const std::shared_ptr<Message>& message) {

    if (message->conversation_id != conversation_id_) {
        return;
    }

    auto message_item_info = CreateMessageItemInfo(message);
    auto insert_iterator = std::upper_bound(
        message_item_infos_.begin(),
        message_item_infos_.end(),
        message_item_info,
        [](const std::shared_ptr<MessageItemInfo>& item1, const std::shared_ptr<MessageItemInfo>& item2) {
            return MessageComparer(item1->message, item2->message);
        }
    );

    insert_iterator = message_item_infos_.insert(insert_iterator, message_item_info);

    std::size_t insert_index = std::distance(message_item_infos_.begin(), insert_iterator);
    NotifyDataAdd(insert_index, 1);
    ScrollToBottom();

    Service::GetInstance().RemoveConversationAllUnreadMessages(conversation_id_);
}


std::size_t ConversationMessageListView::GetDataCount() {
    return message_item_infos_.size();
}


bool ConversationMessageListView::HasVariableItemHeight() {
    return true;
}


float ConversationMessageListView::EstimateItemHeight(
    std::size_t index, 
    const std::shared_ptr<Object>& item_data) {

    if (index >= message_item_infos_.size()) {
        return 0;
    }
    
    return message_item_infos_[index]->message_item->DeterminateHeight(ItemContainer()->ContentSize().width);
}


std::shared_ptr<zaf::ListItem> ConversationMessageListView::CreateItem(
    std::size_t index,
    const std::shared_ptr<Object>& item_data) {

    if (index >= message_item_infos_.size()) {
        return nullptr;
    }

    return message_item_infos_[index]->message_item;
}
