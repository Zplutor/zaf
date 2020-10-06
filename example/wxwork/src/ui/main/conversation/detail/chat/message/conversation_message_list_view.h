#pragma once

#include <zaf/control/list_control.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>
#include "entity/conversation.h"
#include "entity/message.h"
#include "ui/main/conversation/detail/chat/message/message_item.h"

class ConversationMessageListView : 
    public zaf::ListControl, 
    public zaf::ListDataSource, 
    public zaf::ListControlDelegate {

public:
    ~ConversationMessageListView();

    void Initialize() override;

    std::size_t GetDataCount() override;
    bool HasVariableItemHeight() override;

    float EstimateItemHeight(
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) override;

    std::shared_ptr<zaf::ListItem> CreateItem(
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) override;

    void SetConversation(const std::shared_ptr<Conversation>& conversation);

protected:
    void Layout(const zaf::Rect&) override;

private:
    class MessageItemInfo {
    public:
        std::shared_ptr<Message> message;
        std::shared_ptr<MessageItem> message_item;
    };

private:
    std::shared_ptr<MessageItemInfo> CreateMessageItemInfo(const std::shared_ptr<Message>& message);
    void OnMessageAdd(const std::shared_ptr<Message>& message);

private:
    Id conversation_id_ = InvalidId;
    std::vector<std::shared_ptr<MessageItemInfo>> message_item_infos_;
};