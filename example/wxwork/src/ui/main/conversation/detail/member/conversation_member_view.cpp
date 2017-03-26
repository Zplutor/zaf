#include "ui/main/conversation/detail/member/conversation_member_view.h"
#include "logic/service.h"
#include "ui/main/conversation/detail/member/member_item.h"

void ConversationMemberView::Initialize() {

    __super::Initialize();

    SetBackgroundColor(zaf::Color::FromRGB(0xECF0F3));

    SetAllowHorizontalScroll(false);
    SetAutoHideScrollBars(true);

    SetItemSource(std::dynamic_pointer_cast<zaf::ListControl::ItemSource>(shared_from_this()));
}


std::size_t ConversationMemberView::GetItemCount() {
    return members_.size();
}


float ConversationMemberView::GetItemHeight(std::size_t) {
    return 24;
}


std::shared_ptr<zaf::ListControl::Item> ConversationMemberView::CreateItem(std::size_t index) {
    return zaf::Create<MemberItem>();
}


void ConversationMemberView::LoadItem(std::size_t index, const std::shared_ptr<Item>& item) {

    auto member_item = std::dynamic_pointer_cast<MemberItem>(item);
    if (member_item == nullptr) {
        return;
    }

    if (index >= members_.size()) {
        return;
    }

    member_item->SetMember(members_[index]);
}


void ConversationMemberView::SetConversation(const std::shared_ptr<Conversation>& conversation) {

    members_ = Service::GetInstance().GetUsers(conversation->members);
    Reload();
}