#include "conversation_list_view.h"
#include <zaf/control/scroll_bar.h>

void ConversationListView::Initialize() {

    __super::Initialize();

    auto vertical_scroll_bar = GetVerticalScrollBar();
    vertical_scroll_bar->SetSmallChangeValue(64);
    vertical_scroll_bar->SetLargeChangeValue(64 * 10);
}