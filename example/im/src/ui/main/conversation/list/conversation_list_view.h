#pragma once

#include <zaf/control/list_control.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>
#include "ui/main/conversation/conversation_avatar_manager.h"

class ConversationListView : 
    public zaf::ListControl, 
    public zaf::ListDataSource,
    public zaf::ListControlDelegate {

public:
    ~ConversationListView();

    void Initialize() override;

    std::size_t GetDataCount() const override;
    float EstimateItemHeight(std::size_t index, const std::shared_ptr<zaf::dynamic::Object>& data) override;

    std::shared_ptr<zaf::ListItem> CreateItem(
        std::size_t index, 
        const std::shared_ptr<zaf::dynamic::Object>& data) override;

    void SetConversationAvatarManager(const std::shared_ptr<ConversationAvatarManager>& avatar_manager) {
        conversation_avatar_manager_ = avatar_manager;
    }

    void LoadConversations();
    std::shared_ptr<Conversation> GetSelectedConversation();

private:
    void ConversationUpdate(const std::shared_ptr<Conversation>& conversation);

private:
    std::shared_ptr<ConversationAvatarManager> conversation_avatar_manager_;
    std::vector<std::shared_ptr<Conversation>> conversations_;
};