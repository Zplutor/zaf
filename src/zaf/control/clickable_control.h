#pragma once

#include <zaf/control/control.h>
#include <zaf/base/event.h>

namespace zaf {

class ClickableControl : public Control {
public:
	typedef Event<const std::shared_ptr<ClickableControl>&> ClickEvent;

public:
	ClickableControl();
	~ClickableControl();

	bool IsPressed() const {
		return is_pressed_;
	}

	const Color GetPressedBackgroundColor() const {
		if (pressed_background_color_.HasValue()) {
			return pressed_background_color_.GetValue();
		}
		return GetHoveredBackgroundColor();
	}

	void SetPressedBackgroundColor(const Color& color) {
		SetColor(pressed_background_color_, color);
	}

	const Color GetPressedForegroundColor() const {
		if (pressed_foreground_color_.HasValue()) {
			return pressed_foreground_color_.GetValue();
		}
		return GetHoveredForegroundColor();
	}

	void SetPressedForegroundColor(const Color& color) {
		SetColor(pressed_foreground_color_, color);
	}

	const Color GetPressedBorderColor() const {
		if (pressed_border_color_.HasValue()) {
			return pressed_border_color_.GetValue();
		}
		return GetHoveredBorderColor();
	}

	void SetPressedBorderColor(const Color& color) {
		SetColor(pressed_border_color_, color);
	}

public:
	ClickEvent::Proxy OnClick;

protected:
	const Color GetPaintColor(PaintComponent component) const override;

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

	Nullable<Color> pressed_background_color_;
	Nullable<Color> pressed_foreground_color_;
	Nullable<Color> pressed_border_color_;

	ClickEvent click_event_;
};

}