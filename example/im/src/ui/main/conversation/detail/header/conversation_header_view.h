#pragma once

#include <zaf/control/control.h>
#include <zaf/control/label.h>
#include "entity/conversation.h"

class ConversationHeaderView : public zaf::Control {
public:
    void Initialize() override;

    void SetConversation(const std::shared_ptr<Conversation>& conversation);

private:
    std::shared_ptr<zaf::Label> title_label_;
};