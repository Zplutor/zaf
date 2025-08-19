#include <zaf/control/caret.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/dpi.h>
#include <zaf/rx/schedulers/main_thread_scheduler.h>
#include <zaf/rx/timer.h>
#include <zaf/window/window.h>

namespace zaf {

ZAF_OBJECT_IMPL(Caret);


Caret::Caret(const std::weak_ptr<Control>& owner) : owner_(owner) {

}


Caret::~Caret() {

    if (IsVisible()) {
        DestroySystemCaret();
    }
}


void Caret::SetRect(const zaf::Rect& rect) {

    if (rect_ == rect) {
        return;
    }

    //Repaint the old rect to clear the previous caret.
    if (IsVisible()) {
        NeedRepaint();
    }

    rect_ = rect;

    if (IsVisible()) {
        ShowCaret();
    }
}


void Caret::SetIsVisible(bool is_visible) {

    if (is_visible_ == is_visible) {
        return;
    }

    is_visible_ = is_visible;

    if (is_visible_) {
        ShowCaret();
    }
    else {
        HideCaret();
    }
}


void Caret::ShowCaret() {

    //The system caret is not for rendering, but for making IME window follow the input focus.
    CreateSystemCaret();
    NeedRepaint();

    UINT blink_time = GetCaretBlinkTime();
    if (blink_time == 0) {
        blink_time = 1000;
    }

    is_blink_on_ = true;

    auto timer = rx::Timer::Interval(
        std::chrono::milliseconds(blink_time),
        rx::MainThreadScheduler::Instance());

    blink_timer_subscription_ = timer.Subscribe([this](std::size_t) {

        is_blink_on_ = !is_blink_on_;
        NeedRepaint();
    });
}


void Caret::HideCaret() {

    DestroySystemCaret();
    NeedRepaint();

    if (blink_timer_subscription_) {
        blink_timer_subscription_->Dispose();
    }
}


void Caret::NeedRepaint() {

    auto owner = owner_.lock();
    if (!owner) {
        return;
    }

    auto repaint_rect = rect_;
    repaint_rect.AddOffset(owner->ContentRect().position);
    owner->NeedRepaintRect(repaint_rect);
}


void Caret::CreateSystemCaret() {

    auto owner = owner_.lock();
    if (!owner) {
        return;
    }

    auto window = owner->Window();
    if (!window) {
        return;
    }

    auto owner_absolute_content_rect = *owner->ContentRectInWindow();

    auto caret_absolute_rect = Rect();
    caret_absolute_rect.AddOffset(owner_absolute_content_rect.position);
    caret_absolute_rect = FromDIPs(caret_absolute_rect, window->GetDPI());

    CreateCaret(
        window->Handle(),
        nullptr,
        static_cast<int>(caret_absolute_rect.size.width),
        static_cast<int>(caret_absolute_rect.size.height)
    );

    SetCaretPos(
        static_cast<int>(caret_absolute_rect.position.x),
        static_cast<int>(caret_absolute_rect.position.y));
}


void Caret::DestroySystemCaret() {
    DestroyCaret();
}


void Caret::Paint(Canvas& owner_canvas, const zaf::Rect& dirty_rect) const {

    if (!IsVisible()) {
        return;
    }

    if (!is_blink_on_) {
        return;
    }

    if (!rect_.HasIntersection(dirty_rect)) {
        return;
    }

    auto state_guard = owner_canvas.PushState();
    owner_canvas.SetBrushWithColor(Color::Black());
    owner_canvas.DrawRectangle(rect_);
}

}