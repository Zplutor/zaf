#pragma once

#include <zaf/control/textual_control.h>
#include <zaf/base/event.h>

namespace zaf {

class ClickableControl : public TextualControl {
public:
	typedef Event<const std::shared_ptr<ClickableControl>&> ClickEvent;

public:
	ClickableControl();
	~ClickableControl();

	void Initialize() override;

	bool IsPressed() const {
		return is_pressed_;
	}

	ClickEvent::Proxy GetClickEvent();

protected:
	void MouseEnter() override;
	void MouseLeave() override;
    void MouseMove(const Point& position, const MouseMessage& message) override;
    void MouseDown(const Point& position, const MouseMessage& message) override;
    void MouseUp(const Point& position, const MouseMessage& message) override;
	void MouseCapture() override;
	void MouseRelease() override;
    void KeyDown(const KeyMessage& message) override;
    void KeyUp(const KeyMessage& message) override;
	
	virtual void MouseClick();

	void FocusGain() override;
	void FocusLose() override;

private:
	enum class PressType {
		Mouse,
		Key,
	};

private:
	void BeginPress(PressType press_type);
	void EndPress(PressType press_type);
	void CheckIsMousePressed(const Point& position, WPARAM wParam);

private:
	bool is_pressed_;
	bool is_mouse_press_;
	bool is_key_press_;
};

}