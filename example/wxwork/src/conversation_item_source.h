#pragma once

#include <zaf/control/list_control.h>
#include "conversation.h"

class ConversationItemSource : public zaf::ListControl::ItemSource {
public:
    ConversationItemSource();

    std::size_t GetItemCount() override;
    float GetItemHeight(std::size_t index) override;
    std::shared_ptr<zaf::ListControl::Item> CreateItem(std::size_t index) override;
    void LoadItem(std::size_t index, const std::shared_ptr<zaf::ListControl::Item>& item) override;

private:
    std::vector<std::shared_ptr<Conversation>> conversations_;
};