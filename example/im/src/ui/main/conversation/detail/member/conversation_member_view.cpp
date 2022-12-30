#include "ui/main/conversation/detail/member/conversation_member_view.h"
#include "logic/service.h"
#include "ui/main/conversation/detail/member/member_item.h"

void ConversationMemberView::Initialize() {

    __super::Initialize();

    SetBorder(zaf::Frame(1, 0, 0, 0));
    SetBorderColor(zaf::Color::FromRGB(0xD5DDE7));
    SetBackgroundColor(zaf::Color::FromRGB(0xECF0F3));

    SetAllowHorizontalScroll(false);
    SetAutoHideScrollBars(true);

    SetDataSource(std::dynamic_pointer_cast<zaf::ListDataSource>(shared_from_this()));
    SetDelegate(std::dynamic_pointer_cast<zaf::ListControlDelegate>(shared_from_this()));
}


std::size_t ConversationMemberView::GetDataCount() {
    return members_.size();
}


float ConversationMemberView::EstimateItemHeight(
    std::size_t index, 
    const std::shared_ptr<zaf::Object>& data) {

    return 24;
}


std::shared_ptr<zaf::ListItem> ConversationMemberView::CreateItem(
    std::size_t index,
    const std::shared_ptr<zaf::Object>& data) {

    return zaf::Create<MemberItem>();
}


void ConversationMemberView::LoadItem(
    const std::shared_ptr<zaf::ListItem>& item,
    std::size_t item_index) {

    auto member_item = std::dynamic_pointer_cast<MemberItem>(item);
    if (member_item == nullptr) {
        return;
    }

    if (item_index >= members_.size()) {
        return;
    }

    member_item->SetMember(members_[item_index]);
}


void ConversationMemberView::SetConversation(const std::shared_ptr<Conversation>& conversation) {

    members_ = Service::GetInstance().GetUsers(conversation->members);
    Reload();
}