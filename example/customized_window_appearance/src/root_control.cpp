#include "root_control.h"
#include <zaf/control/button.h>
#include <zaf/creation.h>
#include <zaf/window/message/hit_test_message.h>
#include <zaf/window/message/hit_test_result.h>
#include <zaf/window/window.h>

namespace {

const float TitleBarHeight = 30;
const float BorderThickness = 4;

}

void RootControl::Initialize() {

    __super::Initialize();

    SetBackgroundColor(zaf::Color(0.89f, 0.89f, 0.89f));

    close_button_ = zaf::Create<TitleBarButton>(TitleBarButton::Type::Close);
    close_button_->SetRect(zaf::Rect(8, 4, 12, 12));
    Subscriptions() += close_button_->ClickEvent().Subscribe(std::bind([this]() {
        GetWindow()->Close();
    }));
   
    minimize_button_ = zaf::Create<TitleBarButton>(TitleBarButton::Type::Minimize);
    minimize_button_->SetRect(zaf::Rect(28, 4, 12, 12));
    Subscriptions() += minimize_button_->ClickEvent().Subscribe(std::bind([this]() {
        GetWindow()->Minimize();
    }));

    maximize_button_ = zaf::Create<TitleBarButton>(TitleBarButton::Type::Maximize);
    maximize_button_->SetRect(zaf::Rect(48, 4, 12, 12));
    Subscriptions() += maximize_button_->ClickEvent().Subscribe(std::bind([this]() {
        GetWindow()->Maximize();
    }));

    content_control_ = zaf::Create<zaf::Control>();
    content_control_->SetBackgroundColor(zaf::Color::White());

    AddChildren({ close_button_, minimize_button_, maximize_button_, content_control_ });
}


void RootControl::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    auto size = GetSize();
    content_control_->SetRect(zaf::Rect(
        BorderThickness, 
        TitleBarHeight, 
        size.width - BorderThickness * 2, 
        size.height - TitleBarHeight - BorderThickness));
}


std::optional<zaf::HitTestResult> RootControl::HitTest(const zaf::HitTestMessage& message) {

    auto mouse_position = GetMousePosition();

    if (close_button_->Rect().Contain(mouse_position)) {
        return zaf::HitTestResult::CloseButton;
    }

    if (minimize_button_->Rect().Contain(mouse_position)) {
        return zaf::HitTestResult::MinimizeButton;
    }

    if (maximize_button_->Rect().Contain(mouse_position)) {
        return zaf::HitTestResult::MaximizeButton;
    }

    zaf::Rect top_left_corner_rect(0, 0, BorderThickness, BorderThickness);
    if (top_left_corner_rect.Contain(mouse_position)) {
        return zaf::HitTestResult::TopLeftCorner;
    }

    zaf::Rect top_right_corner_rect(
        GetWidth() - BorderThickness, 
        0,
        BorderThickness,
        BorderThickness);
    if (top_right_corner_rect.Contain(mouse_position)) {
        return zaf::HitTestResult::TopRightCorner;
    }

    zaf::Rect bottom_left_corner_rect(
        0, 
        GetHeight() - BorderThickness,
        BorderThickness,
        BorderThickness);
    if (bottom_left_corner_rect.Contain(mouse_position)) {
        return zaf::HitTestResult::BottomLeftCorner;
    }

    zaf::Rect bottom_right_corner_rect(
        GetWidth() - BorderThickness,
        GetHeight() - BorderThickness,
        BorderThickness,
        BorderThickness);
    if (bottom_right_corner_rect.Contain(mouse_position)) {
        return zaf::HitTestResult::BottomRightCorner;
    }

    zaf::Rect top_border_rect(0, 0, GetWidth(), BorderThickness);
    if (top_border_rect.Contain(mouse_position)) {
        return zaf::HitTestResult::TopBorder;
    }

    zaf::Rect left_border_rect(0, TitleBarHeight, BorderThickness, GetHeight());
    if (left_border_rect.Contain(mouse_position)) {
        return zaf::HitTestResult::LeftBorder;
    }

    zaf::Rect right_border_rect(
        GetWidth() - BorderThickness, 
        TitleBarHeight, 
        BorderThickness, 
        GetHeight());
    if (right_border_rect.Contain(mouse_position)) {
        return zaf::HitTestResult::RightBorder;
    }

    zaf::Rect bottom_border_rect(0, GetHeight() - BorderThickness, GetWidth(), BorderThickness);
    if (bottom_border_rect.Contain(mouse_position)) {
        return zaf::HitTestResult::BottomBorder;
    }

    zaf::Rect title_bar_rect(0, 0, GetWidth(), TitleBarHeight);
    if (title_bar_rect.Contain(mouse_position)) {
        return zaf::HitTestResult::TitleBar;
    }

    return __super::HitTest(message);
}