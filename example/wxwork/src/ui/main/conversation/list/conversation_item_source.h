#pragma once

#include <zaf/control/list_control.h>
#include "entity/conversation.h"
#include "ui/main/conversation/conversation_avatar_manager.h"

class ConversationItemSource : public zaf::ListControl::ItemSource {
public:
    ConversationItemSource();
    ~ConversationItemSource();

    void LoadConversations(const std::shared_ptr<ConversationAvatarManager>& avatar_manager);

    std::size_t GetItemCount() override;
    float GetItemHeight(std::size_t index) override;
    std::shared_ptr<zaf::ListControl::Item> CreateItem(std::size_t index) override;
    void LoadItem(std::size_t index, const std::shared_ptr<zaf::ListControl::Item>& item) override;

private:
    void ConversationUpdate(const std::shared_ptr<Conversation>& conversation);

private:
    std::vector<std::shared_ptr<Conversation>> conversations_;
    std::shared_ptr<ConversationAvatarManager> conversation_avatar_manager_;
};