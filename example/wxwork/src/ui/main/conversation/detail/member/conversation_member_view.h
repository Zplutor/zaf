#pragma once

#include <zaf/control/list_control.h>
#include "entity/conversation.h"

class ConversationMemberView : public zaf::ListControl, public zaf::ListItemSource {
public:
    void Initialize() override;

    std::size_t GetItemCount() override;
    float GetItemHeight(std::size_t) override;
    std::shared_ptr<zaf::ListItem> CreateItem(std::size_t index) override;
    void LoadItem(std::size_t index, const std::shared_ptr<zaf::ListItem>& item) override;

    void SetConversation(const std::shared_ptr<Conversation>& conversation);

private:
    std::vector<std::shared_ptr<User>> members_;
};