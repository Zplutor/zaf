#include "main_window.h"
#include <zaf/control/layout/array_layouter.h>
#include "ui/main/conversation/conversation_panel.h"

void MainWindow::Initialize() {

    __super::Initialize();

    auto conversation_panel = zaf::Create<ConversationPanel>();
    conversation_panel_ = conversation_panel;

    content_panel_container_ = zaf::Create<zaf::Control>();
    content_panel_container_->SetLayouter(zaf::GetHorizontalArrayLayouter());
    content_panel_container_->AddChild(conversation_panel);
    
    auto root_control = GetRootControl();
    root_control->SetLayouter(zaf::GetHorizontalArrayLayouter());
    root_control->AddChild(conversation_panel);

    conversation_panel_->LoadContent();
}