#pragma once

#include <zaf/control/list_control.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>
#include "entity/conversation.h"

class ConversationMemberView : 
    public zaf::ListControl, 
    public zaf::ListDataSource,
    public zaf::ListControlDelegate {

public:
    void Initialize() override;

    std::size_t GetDataCount() override;
    float EstimateItemHeight(std::size_t index, const std::shared_ptr<zaf::Object>& data) override;

    std::shared_ptr<zaf::ListItem> CreateItem(
        std::size_t index, 
        const std::shared_ptr<zaf::Object>& data) override;

    void LoadItem(const std::shared_ptr<zaf::ListItem>& item, std::size_t item_index) override;

    void SetConversation(const std::shared_ptr<Conversation>& conversation);

private:
    std::vector<std::shared_ptr<User>> members_;
};