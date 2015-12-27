#include <zaf/window/caret.h>
#include <zaf/canvas.h>
#include <zaf/base/timer.h>
#include <zaf/window/window.h>

namespace zaf {

Caret::Caret() : 
	is_visible_(false) {

}


Caret::~Caret() {
	Hide();
}


void Caret::SetRect(const Rect& rect) {

	Rect previous_rect = GetRect();
	rect_ = rect;
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

	blink_timer_ = std::make_unique<Timer>(
		blink_time,
		[this](Timer&) {
			NeedRepaint(!is_visible_);
		}
	);
	blink_timer_->Start();
}


void Caret::Hide() {

	if (! IsShown()) {
		return;
	}

	blink_timer_.reset();
	NeedRepaint(false);
}


void Caret::NeedRepaint(bool bling_visible) {

	is_visible_ = bling_visible;

	auto window = window_.lock();
	if (window != nullptr) {
		window->NeedRepaintRect(GetRect());
	}
}


void Caret::Paint(Canvas& canvas) {

	if (! is_visible_) {
		return;
	}

	canvas.BeginPaint();
	canvas.SetBrushColor(Color::Black);
	canvas.DrawRectangle(GetRect());
	canvas.EndPaint();
}

}