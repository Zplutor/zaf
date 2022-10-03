#include "ui/main/conversation/common/title_generating.h"
#include <algorithm>
#include "logic/service.h"

std::wstring GenerateConversationTitle(const std::shared_ptr<Conversation>& conversation) {

    if (! conversation->title.empty()) {
        return conversation->title;
    }

    if (conversation->members.size() < 2) {
        return L"(No title)";
    }

    auto& service = Service::GetInstance();
    auto members = service.GetUsers(conversation->members);
    if (members.size() == 2) {

        auto current_user_id = service.GetCurrentUserId();
        auto display_member = members[0]->id == current_user_id ? members[1] : members[0];
        return display_member->name;
    }

    std::sort(
        members.begin(),
        members.end(),
        [](const std::shared_ptr<User>& user1, const std::shared_ptr<User>& user2) {
        return user1->name < user2->name;
    }
    );

    std::wstring title;
    for (const auto& each_member : members) {
        title.append(each_member->name);
        if (each_member != members.back()) {
            title.append(L"; ");
        }
    }
    return title;
}