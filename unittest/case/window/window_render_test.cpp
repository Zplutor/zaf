#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include <zaf/control/linear_box.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>
#include "window_test.h"

namespace zaf::testing {
namespace {

class TestControl : public zaf::VerticalBox {
protected:
	void Initialize() override {
		__super::Initialize();
		repaint_child_ = zaf::Create<zaf::Control>();
        this->AddChild(repaint_child_);
	}

	void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const override {
		__super::Paint(canvas, dirty_rect);
		repaint_child_->NeedRepaint();
	}

private:
	std::shared_ptr<zaf::Control> repaint_child_;
};

}

/*
It is not allowed to request another repaint while painting, as it will cause infinite repainting.
The illegal request will be ignored. This test verifies it.
*/
TEST_F(WindowTest, RequestRepaintWhenPainting) {

	auto window = zaf::Create<zaf::Window>();
	window->SetRect(zaf::Rect{ 0, 0, 100, 100 });

	auto control = zaf::Create<TestControl>();
	control->SetRect(zaf::Rect{ 0, 0, 100, 100 });

	window->SetRootControl(control);

	int paint_message_count{};
	auto sub = window->MessageReceivedEvent().Subscribe(
		[&paint_message_count](const zaf::MessageReceivedInfo& event_info) {
			if (event_info.Message().ID() == WM_PAINT) {
				++paint_message_count;
			}
		});

	window->Show();

	// Consume initial paint requirement and counters.
	window->RepaintIfNeeded();
	paint_message_count = 0;

    InvalidateRect(window->Handle(), nullptr, FALSE);
	window->RepaintIfNeeded();
	window->RepaintIfNeeded();
	window->RepaintIfNeeded();

	ASSERT_EQ(paint_message_count, 1);
	window->Destroy();
}

}