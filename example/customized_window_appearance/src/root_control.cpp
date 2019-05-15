#include "root_control.h"
#include <zaf/control/button.h>
#include <zaf/creation.h>
#include <zaf/window/message/hit_test_message.h>
#include <zaf/window/message/hit_test_result.h>

void RootControl::Initialize() {

    __super::Initialize();

    close_button_ = zaf::Create<zaf::Button>();
    close_button_->SetSize(zaf::Size(30, 30));
    close_button_->SetPosition(zaf::Point(0, 0));
    close_button_->SetText(L"X");

    AddChild(close_button_);
}


std::optional<zaf::HitTestResult> RootControl::HitTest(const zaf::HitTestMessage& message) {

    zaf::Rect title_bar_rect(0, 0, GetWidth(), 30);

    if (close_button_->GetRect().Contain(close_button_->GetMousePosition())) {
        return zaf::HitTestResult::CloseButton;
    }

    if (title_bar_rect.Contain(GetMousePosition())) {
        return zaf::HitTestResult::TitleBar;
    }

    return __super::HitTest(message);
}