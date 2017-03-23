#include "conversation_list_view.h"
#include <zaf/control/scroll_bar.h>
#include "ui/main/conversation/list/conversation_item_source.h"

void ConversationListView::Initialize() {

    __super::Initialize();

    auto vertical_scroll_bar = GetVerticalScrollBar();
    vertical_scroll_bar->SetSmallChangeValue(64);
    vertical_scroll_bar->SetLargeChangeValue(64 * 10);
}


void ConversationListView::LoadConversations(const std::shared_ptr<ConversationAvatarManager>& avatar_manager) {

    auto item_source = std::make_shared<ConversationItemSource>();
    item_source->LoadConversations(avatar_manager);
    SetItemSource(item_source);
}