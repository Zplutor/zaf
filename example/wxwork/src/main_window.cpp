#include "main_window.h"
#include <zaf/control/layout/array_layouter.h>
#include "conversation_item_source.h"

void MainWindow::Initialize() {

    __super::Initialize();

    auto conversation_item_source = std::make_shared<ConversationItemSource>();

    conversation_list_view_ = zaf::Create<ConversationListView>();
    conversation_list_view_->SetItemSource(conversation_item_source);

    auto root_control = GetRootControl();
    root_control->SetLayouter(zaf::GetHorizontalArrayLayouter());
    root_control->AddChild(conversation_list_view_);
}