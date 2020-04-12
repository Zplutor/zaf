#include "main_window.h"
#include <zaf/control/layout/linear_layouter.h>
#include "ui/main/conversation/conversation_panel.h"

void MainWindow::Initialize() {

    __super::Initialize();

    SetInitialRectStyle(InitialRectStyle::CenterInScreen);
    SetSize(zaf::Size(800, 600));

    auto conversation_panel = zaf::Create<ConversationPanel>();
    conversation_panel_ = conversation_panel;

    content_panel_container_ = zaf::Create<zaf::Control>();
    content_panel_container_->SetLayouter(zaf::LinearLayouter::CreateLeftToRightLayouter());
    content_panel_container_->AddChild(conversation_panel);
    
    auto root_control = GetRootControl();
    root_control->SetLayouter(zaf::LinearLayouter::CreateLeftToRightLayouter());
    root_control->AddChild(conversation_panel);

    conversation_panel_->LoadContent();
}