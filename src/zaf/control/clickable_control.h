#pragma once

#include <zaf/control/textual_control.h>
#include <zaf/base/event.h>

namespace zaf {

/**
 Represents a clickable control.   

 This is the base class of all controls that support click action.
 */
class ClickableControl : public TextualControl {
public:
    ZAF_DECLARE_REFLECTION_TYPE

public:
    /**
     Type of click event.
     */
	typedef Event<const std::shared_ptr<ClickableControl>&> ClickEvent;

public:
	ClickableControl();
	~ClickableControl();

    /**
     Get a value indicating that whether the control is being pressed.
     */
	bool IsPressed() const {
		return is_pressed_;
	}

    /**
     Get the click event.
     */
	ClickEvent::Proxy GetClickEvent();

    /**
     Perform a click action on the control.
     */
    void Click();

protected:
    void Initialize() override;
	void MouseEnter(const std::shared_ptr<Control>& entered_control) override;
    void MouseLeave(const std::shared_ptr<Control>& leaved_control) override;
    bool MouseMove(const Point& position, const MouseMessage& message) override;
    bool MouseDown(const Point& position, const MouseMessage& message) override;
    bool MouseUp(const Point& position, const MouseMessage& message) override;
	void MouseCapture() override;
	void MouseRelease() override;
    bool KeyDown(const KeyMessage& message) override;
    bool KeyUp(const KeyMessage& message) override;
	
    /**
     This method is called when a click action is performed on the control.

     Derived classes should call the same method of super class if they don't 
     handle the event.
     */
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
	void CheckIsMousePressed(const Point& position, const MouseMessage& message);

private:
	bool is_pressed_;
	bool is_mouse_press_;
	bool is_key_press_;
};

}