#pragma once

#include <zaf/control/split_control.h>
#include "ui/main/conversation/detail/member/conversation_member_view.h"

class ConversationDetailView : public zaf::Control {
public:
    void Initialize() override;

    void SetConversation(const std::shared_ptr<Conversation>& conversation);

protected:
    void Layout(const zaf::Rect&) override;

private:
    void LayoutMemberView();

private:
    std::shared_ptr<ConversationMemberView> member_view_;
};