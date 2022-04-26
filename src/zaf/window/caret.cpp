#include <zaf/window/caret.h>
#include <zaf/base/timer.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/dpi.h>
#include <zaf/window/window.h>

namespace zaf {

Caret::Caret() : 
    is_visible_(false) {

}


Caret::~Caret() {
    Hide();
}


void Caret::SetRect(const Rect& rect) {

    rect_ = rect;

    if (IsShown()) {
        CreateSystemCaret();
    }
}


void Caret::Show() {

    if (IsShown()) {
        return;
    }

    NeedRepaint(true);

    UINT blink_time = GetCaretBlinkTime();
    if (blink_time == 0) {
        blink_time = 1000;
    }

    blink_timer_ = std::make_shared<Timer>(Timer::Mode::DeferredRepeated);
    blink_timer_->SetInterval(std::chrono::milliseconds(blink_time));
    Subscriptions() += blink_timer_->TriggerEvent().Subscribe(std::bind([this]() {
        NeedRepaint(!is_visible_);
    }));
    blink_timer_->Start();

    CreateSystemCaret();
}


void Caret::CreateSystemCaret() {

    auto window = window_.lock();
    if (window == nullptr) {
        return;
    }

    const auto& caret_rect = FromDIPs(GetRect(), window->GetDPI());
    CreateCaret(
        window->Handle(), 
        nullptr, 
        static_cast<int>(caret_rect.size.width),
        static_cast<int>(caret_rect.size.height)
    );

    SetCaretPos(static_cast<int>(caret_rect.position.x), static_cast<int>(caret_rect.position.y));
}


void Caret::Hide() {

    if (! IsShown()) {
        return;
    }

    blink_timer_.reset();
    NeedRepaint(false);

    DestroySystemCaret();
}


void Caret::DestroySystemCaret() {
    DestroyCaret();
}


void Caret::NeedRepaint(bool bling_visible) {

    is_visible_ = bling_visible;

    auto window = window_.lock();
    if (window != nullptr) {
        window->NeedRepaintRect(GetRect());
    }
}


void Caret::Repaint(Canvas& canvas) {

    if (! is_visible_) {
        return;
    }

    canvas.BeginPaint();
    canvas.SaveState();

    canvas.SetBrushWithColor(Color::Black());
    canvas.DrawRectangle(GetRect());

    canvas.RestoreState();
    canvas.EndPaint();
}

}