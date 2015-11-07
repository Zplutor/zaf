#pragma once

#include "control.h"
#include "event.h"

namespace zaf {

class Button : public Control {
public:
	typedef Event<const std::shared_ptr<Button>&> ClickEvent;
	
public:
	Button();
	~Button();

	bool IsPressed() const {
		return is_pressed_;
	}

public:
	ClickEvent::Proxy OnClick;

protected:
	void Paint(Canvas& canvas, const Rect& dirty_rect) override;

	void MouseMove(const Point& position, WPARAM wParam, LPARAM lParam) override;
	void MouseDown(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam) override;
	void MouseUp(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam) override;
	void MouseCapture() override;
	void MouseRelease() override;

private:
	void CheckIsPressed(const Point& position, WPARAM wParam);

private:
	bool is_pressed_;
	ClickEvent click_event_;
};

}