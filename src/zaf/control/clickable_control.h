#pragma once

#include <zaf/control/control.h>
#include <zaf/base/event.h>

namespace zaf {

class ClickableControl : public Control {
public:
	class PaintState : public Control::PaintState {
	public:
		static const int Pressed = Control::PaintState::Custom + 0;
		static const int Custom = Control::PaintState::Custom + 100;
	};

	typedef Event<const std::shared_ptr<ClickableControl>&> ClickEvent;

public:
	ClickableControl();
	~ClickableControl();

	bool IsPressed() const {
		return is_pressed_;
	}

public:
	ClickEvent::Proxy OnClick;

protected:
	void Initialize() override;

	int GetPaintState() const override;

	const Color GetDefaultColor(int paint_component, int paint_state) const override;

	void MouseEnter() override;
	void MouseLeave() override;
	void MouseMove(const Point& position, WPARAM wParam, LPARAM lParam) override;
	void MouseDown(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam) override;
	void MouseUp(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam) override;
	void MouseCapture() override;
	void MouseRelease() override;
	void KeyDown(WPARAM wParam, LPARAM lParam) override;
	void KeyUp(WPARAM wParam, LPARAM lParam) override;
	
	virtual void MouseClick();

	void FocusGain() override;
	void FocusLose() override;

private:
	enum class PressType {
		Mouse,
		Key,
	};

private:
	const Color GetPaintColor(
		const Color(ClickableControl::*get_pressed_color)() const,
		const Color(Control::*get_base_color)() const
	) const;

	void BeginPress(PressType press_type);
	void EndPress(PressType press_type);
	void CheckIsMousePressed(const Point& position, WPARAM wParam);

private:
	bool is_pressed_;
	bool is_mouse_press_;
	bool is_key_press_;

	ClickEvent click_event_;
};

}