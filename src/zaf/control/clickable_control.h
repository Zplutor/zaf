#pragma once

#include <zaf/control/event/click_info.h>
#include <zaf/control/textual_control.h>
#include <zaf/rx/observable.h>

namespace zaf {

/**
 Represents a clickable control.   

 This is the base class of all controls that support click action.
 */
class ClickableControl : public TextualControl {
public:
    ZAF_DECLARE_TYPE

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
     Perform a click action on the control.
     */
    void Click();

    Observable<ClickInfo> ClickEvent();

protected:
    void Initialize() override;
    void OnMouseEnter(const MouseEnterInfo& event_info) override;
    void OnMouseLeave(const MouseLeaveInfo& event_info) override;
    void OnMouseMove(const MouseMoveInfo& event_info) override;
    void OnMouseDown(const MouseDownInfo& event_info) override;
    void OnMouseUp(const MouseUpInfo& event_info) override;
    void OnMouseCapture() override;
    void OnMouseRelease() override;
    void OnKeyDown(const KeyDownInfo& event_info) override;
    void OnKeyUp(const KeyUpInfo& event_info) override;
    void OnFocusGained(const FocusGainedInfo& event_info) override;
    void OnFocusLost(const FocusLostInfo& event_info) override;

    /**
    Handles click notification. This method would be call when the control is clicked.

    The default implementation raises click event. Derived classes should call the same method of
    base class to raise the event if they override it.
    */
    virtual void OnClick(const ClickInfo& event_info);

private:
    enum class PressType {
        Mouse,
        Key,
    };

private:
    void BeginPress(PressType press_type);
    void EndPress(PressType press_type);
    bool CheckIsMousePressed(const Point& position, const MouseMessage& message);

private:
    bool is_pressed_;
    bool is_mouse_press_;
    bool is_key_press_;
};

}